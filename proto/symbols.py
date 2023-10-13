#!/bin/python3
"""
symbols.py
---

Author: Keenan W. Wilkinson
Date: 29 Sep 2023
---

Defines objects which parses out streams of data into symbols more
digestable by some other process. Lexical analysis or AST parsing for
example.
"""

import abc
import io
import os
import string
import typing

T = typing.TypeVar("T")
T_co = typing.TypeVar("T_co", covariant=True)
T_ct = typing.TypeVar("T_ct", contravariant=True)
Column = typing.TypeVar("Column", bound=int)
Lineno = typing.TypeVar("Lineno", bound=int)
Symbol = bytearray

COMMENT_CHAR = b"#"
DIGIT_CHARS = string.digits.encode()
# With digits extends digits to allow up to base
# 91 (BasE91) numbers.
DIGIT_CHARS_EXT = (
    (string.ascii_letters + string.punctuation)
    .replace(r"-", "")
    .replace("\\", "")
    .replace("'", "")).encode()
DIGIT_CHARS_HEX = string.hexdigits.encode()
DIGIT_SEP_CHARS = b".xdbo"
END_OF_FILE = bytearray(b"EOF")
END_OF_LINE = bytearray(b"EOL")
NAME_CHARS = (string.ascii_letters + string.digits + "_").encode()
NEWLINE_CHAR = os.linesep.encode()
STR_CHARS = b"'`\""
STR_SYMBOLS = (
    bytearray(b"\""),
    bytearray(b"\""*3),
    bytearray(b"'"),
    bytearray(b"'"*3),
    bytearray(b"`"),
    bytearray(b"`"*3)
)
STRUCTURE_CHARS = b"])}{(["
TERM_CHARS = b";"
WHITESPACE = string.whitespace.encode()


class SymbolParser(typing.Protocol[T_co]):
    """
    Parses a buffer of data into symbols that are
    usable for token parsing.
    """

    @abc.abstractmethod
    def __iter__(self) -> typing.Generator[T_co, None, None]:
        pass

    @abc.abstractmethod
    def end(self) -> bool:
        """
        Read head is at the end of data stream or
        not.
        """

    @abc.abstractmethod
    def head(self) -> int:
        """Character at read head."""

    @abc.abstractmethod
    def next(self) -> T_co:
        """Parse next symbol."""


class BasicSymbolParser(SymbolParser[T]):
    """
    Parses generic symbols into a tuple of
    `(line_number, start_column, symbol)`.
    """

    data:             bytes
    dimension_line:   int
    file:             bytes | None
    last_line_at:     int
    read_head:        int
    string_parsing:   bool
    symbol_history:   tuple[bytearray, bytearray, bytearray]

    def __iter__(self):
        while not self.end():
            yield self.next()

    def __init__(
        self,
        data: bytes | str | io.BufferedReader | None = None,
        *,
        file: bytes | str | None = None):

        self.dimension_line = 1 #type: ignore[assignment]
        self.last_line_at = 0
        self.read_head = 0
        self.string_parsing = False
        self.symbol_history = (bytearray(), bytearray(), bytearray())

        if not data and not file:
            raise ValueError("No data, stream or file path provided.")

        if isinstance(file, str):
            self.file = file.encode()
        else:
            self.file = file

        if isinstance(data, str):
            self.data = data.encode()
        elif isinstance(data, io.BufferedReader):
            self.file = data.name.encode()
            self.data = data.read()
        elif self.file:
            with open(self.file, "rb") as fd:
                self.data = fd.read()
        else:
            self.data = data or b""

    @property
    def last_symbol(self):
        return self.symbol_history[-1]

    def advance(self):
        """Move the read head forward."""

        if char_isnewline(self.head()):
            self.dimension_line += 1 #type: ignore
            self.last_line_at = self.read_head

        self.read_head += 1

    def advance_whitespace(self):
        """
        Move the read head to next valid
        non-whitespace character.
        """

        if self.string_parsing:
            return

        while char_isnoparse(self.head()) and not self.end():
            self.advance()

    def advance_comments(self):
        """
        Move the read head to next valid
        non-comment character.
        """

        if self.string_parsing:
            return

        while char_iscomment(self.head()):
            # Comments cannot exist inline with code.
            # The end of comments are determined
            # based on the end of line or EOF.
            while not char_isnewline(self.head()) and not self.end():
                self.advance()

            # Second pass ensures additional whitespace
            # after a comment is eliminated.
            self.advance_whitespace()

    def end(self):
        return self.read_head >= len(self.data)

    def head(self):
        # Fixes a bug where iterating to `the end`
        # does not always return the last symbol.
        # In particular, when the final symbol is
        # 1 char long.
        if self.end():
            return self.data[-1]
        return self.data[self.read_head]

    def lineno(self):
        """The current line number."""

        return self.dimension_line

    def lookahead(self, head: int):
        """
        Get slice of bytes of `head` length of
        data stream relative to read head.
        """

        return memoryview(self.data)[self.read_head:self.read_head+head]

    def lookahead_matchlast(self):
        """
        The last symbol parsed is equal to the
        lookahead slice.
        """

        return self.lookahead(len(self.last_symbol)) == self.last_symbol

    def next(self):
        # Advance past all whitespace and chars
        # considered invalid for parsing.
        # This includes characters after a comment
        # char (#) and newline chars.
        self.advance_whitespace()
        self.advance_comments()

        # This should always be checked to prevent
        # bone-headed calls to 'next' when there's
        # nothing left to parse.
        if self.end():
            symbol = END_OF_FILE if self.lineno() > 1 else END_OF_LINE
            return self.lineno(), 0, symbol

        # Determine what we are most likely parsing.
        if self.string_parsing:
            nexter = self.next_punc
        elif char_isnamechar(self.head()) and not char_isdigitchar(self.head()):
            nexter = self.next_name
        elif char_isdigitchar(self.head()):
            nexter = self.next_numeric
        elif char_isdigitsep(self.head()) and char_isdigitchar(self.lookahead(2)[1]):
            nexter = self.next_numeric
        else:
            nexter = self.next_punc

        token = nexter()
        self.symbol_history = (*self.symbol_history[-2:], token[2])

        return token

    def next_name(self):
        """Parse next name symbol."""

        symbol = bytearray()
        column = (self.read_head - self.last_line_at)

        while True:
            symbol.append(self.head())
            self.advance()

            if char_isnoparse(self.head()):
                break
            if char_iscomment(self.head()):
                break
            if char_istermchar(self.head()):
                break
            # Punctuation cannot exist in a name.
            # Unless said name is numeric, then
            # '.' is accepted.
            if not symbol_isvalidname(symbol, self.head()):
                break

            if self.end():
                break

        return self.lineno(), column, symbol

    def next_numeric(self):
        """Parse next numeric symbol."""

        symbol = bytearray()
        column = (self.read_head - self.last_line_at)

        while True:
            symbol.append(self.head())
            self.advance()

            if char_isnoparse(self.head()):
                break
            if char_iscomment(self.head()):
                break
            if char_istermchar(self.head()):
                break
            if not symbol_isvalidnum(symbol, self.head()):
                break

            if self.end():
                break

        return self.lineno(), column, symbol

    def next_punc(self):
        """Parse next punctuation symbol."""

        symbol = bytearray()
        column = (self.read_head - self.last_line_at)

        while True:
            symbol.append(self.head())
            self.advance()

            if not self.string_parsing:
                if char_isnoparse(self.head()):
                    break
                if char_iscomment(self.head()):
                    break
                if char_isstructchar(self.head()):
                    break
                if char_istermchar(self.head()):
                    break

                # Gratuitious most likely.
                if char_isstructchar(symbol):
                    break

                # Names cannot exist in punctuation.
                if not symbol_isvalidpunc(symbol, self.head()):
                    break

                # String parsing has most likely
                # started. Valid string tokens
                # must not contain non-string
                # notation chars.
                if symbol_isstrsym(symbol) and not char_isstrchar(self.head()):
                    break

            # If string parsing, ignore whatever
            # character might come next as it is
            # most likely being escaped.
            elif symbol[-1] in b"\\":
                continue

            # If future char sequence is the same
            # as the opening string sequence.
            elif self.lookahead_matchlast():
                break

            # If the symbol matches the opening
            # string sequence.
            elif self.symbol_history[1] == symbol and symbol_isstrsym(self.symbol_history[1]):
                break

            if self.end():
                break

        if symbol_isstrsym(symbol):
            self.string_parsing = not self.string_parsing

        return self.lineno(), column, symbol


def char_iscomment(char: bytes | int):
    return char in COMMENT_CHAR


def char_isdigitchar(char: bytes | int):
    return char in DIGIT_CHARS


def char_isdigitsep(char: bytes | int):
    return char in DIGIT_SEP_CHARS


def char_isnamechar(char: bytes | int):
    return char in NAME_CHARS


def char_isnewline(char: bytes | int):
    return char in NEWLINE_CHAR


def char_isnoparse(char: bytes | int):
    return char in WHITESPACE


def char_ispuncchar(char: bytes | int):
    return char not in NAME_CHARS


def char_istermchar(char: bytes | int):
    return any([char == tc for tc in TERM_CHARS])


def char_isstrchar(char: bytes | int):
    return char in STR_CHARS


def char_isstructchar(char: bytes | int):
    return char in STRUCTURE_CHARS


def symbol_isname(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] in NAME_CHARS
        and symbol[0] not in DIGIT_CHARS
        and symbol[-1] in NAME_CHARS)


def symbol_isnumeric(symbol: bytearray):
    # Byte array might be numeric if  any chars
    # in the sequence are digits.
    if not bool(symbol and any([i in DIGIT_CHARS for i in symbol])):
        return False

    base_notation = 0
    # Byte array is numeric if there is not mixing
    # of floating point ('.') char and special
    # base notations (0x, 0d, 0b, 0o).
    for ch in DIGIT_SEP_CHARS[1:]:
        if ch in symbol and b"." in symbol:
            return False
        elif ch in symbol:
            base_notation = ch

    # Byte array is numeric if only digits are
    # found in the sequence.
    for ch in symbol:
        # If parsing for base 10, binary or octal.
        if base_notation in (0, 98, 111):
            if ch not in DIGIT_CHARS + DIGIT_SEP_CHARS:
                return False
        # If parsing hex or some other base.
        else:
            if ch not in DIGIT_CHARS + DIGIT_CHARS_EXT:
                return False

    return (symbol.count(b".") < 2)


def symbol_ispunc(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] not in NAME_CHARS
        and symbol[-1] not in NAME_CHARS)


def symbol_isstrsym(symbol: bytearray):
    return bool(symbol and symbol in STR_SYMBOLS)


def symbol_istermed(symbol: bytearray, next_char: bytes | int):
    return bool(
        symbol
        and any([tc not in symbol for tc in TERM_CHARS])
        and char_istermchar(next_char))


def symbol_isvalidname(symbol: bytearray, next_char: bytes | int):
    return (symbol_isname(symbol) and not char_ispuncchar(next_char))


def symbol_isvalidnum(symbol: bytearray, next_char: bytes | int):
    if not symbol_isnumeric(symbol) and symbol != b".":
        return False

    if b"." in symbol:
        return not char_ispuncchar(next_char)
    elif char_isdigitchar(next_char):
        return True
    elif char_isdigitsep(next_char):
        return True
    elif not char_ispuncchar(next_char):
        return True

    return False


def symbol_isvalidpunc(symbol: bytearray, next_char: bytes | int):
    return not (symbol_ispunc(symbol) and char_isnamechar(next_char))


def main():
    symbols_parsed = []
    symbols_tested = [
        (5, 1, bytearray(b'sx')),
        (5, 3, bytearray(b':')),
        (5, 5, bytearray(b'int')),
        (5, 9, bytearray(b'=')),
        (5, 11, bytearray(b'0')),
        (5, 12, bytearray(b';')),
        (6, 1, bytearray(b'cx')),
        (6, 3, bytearray(b':')),
        (6, 5, bytearray(b'str')),
        (6, 9, bytearray(b'=')),
        (6, 11, bytearray(b"\'\'\'")),
        (6, 14, bytearray(b"d%\'-\'`")),
        (6, 20, bytearray(b"\'\'\'")),
        (6, 23, bytearray(b';')),
        (7, 1, bytearray(b'kv')),
        (7, 3, bytearray(b':')),
        (7, 5, bytearray(b'str')),
        (7, 9, bytearray(b'=')),
        (7, 11, bytearray(b'"')),
        (7, 12, bytearray(b'{interpol} this')),
        (7, 27, bytearray(b'"')),
        (7, 28, bytearray(b';')),
        (8, 1, bytearray(b'x')),
        (8, 2, bytearray(b'++')),
        (8, 4, bytearray(b';')),
        (8, 6, bytearray(b's')),
        (8, 7, bytearray(b':')),
        (8, 9, bytearray(b'flt')),
        (8, 13, bytearray(b'=')),
        (8, 15, bytearray(b'49.9')),
        (8, 19, bytearray(b'.3')),
        (8, 21, bytearray(b';')),
        (11, 0, bytearray(b'EOF'))
    ]

    with open("tests/test_symbols.vxn", "rb") as fd:
        for symbol in BasicSymbolParser(fd):
            symbols_parsed.append(symbol)

    for st, sp in zip(symbols_tested, symbols_parsed):
        print(st, sp, sep="  \t\t")
        assert st == sp, f"Received invalid token {sp!r}."

    return 0


if __name__ == "__main__":
    exit(main())
