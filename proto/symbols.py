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
Column = typing.TypeVar("Column", bound=int)
Lineno = typing.TypeVar("Lineno", bound=int)
Symbol = bytearray

COMMENT_CHAR = b"#"
DIGIT_CHARS = string.digits.encode()
DIGIT_SEP_CHARS = b".xbo"
END_OF_FILE = bytearray(b"EOF")
END_OF_LINE = bytearray(b"EOL")
NAME_CHARS = (string.ascii_letters + string.digits + "_").encode()
NEWLINE_CHAR = os.linesep.encode()
STR_CHARS = b"'\""
STR_SYMBOLS = (
    bytearray(b"\""),
    bytearray(b"\""*3),
    bytearray(b"'"),
    bytearray(b"'"*3),
    bytearray(b"`"),
    bytearray(b"`"*3)
)
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


class BasicSymbolParser(SymbolParser[tuple[Lineno, Column, Symbol]]):
    """
    Parses generic symbols into a tuple of
    `(line_number, start_column, symbol)`.
    """

    data:             bytes
    dimension_line:   Lineno
    file:             bytes | None
    last_line_at:     int
    last_symbol:      bytearray
    read_head:        int
    string_parsing:   bool

    def __iter__(self):
        while not self.end():
            yield self.next()

    def __init__(self, data: bytes | str | io.BufferedReader):

        self.dimension_line = 1 #type: ignore[assignment]
        self.file = None
        self.last_line_at = 0
        self.last_symbol = bytearray()
        self.read_head = 0
        self.string_parsing = False

        if isinstance(data, str):
            self.data = data.encode()
        elif isinstance(data, io.BufferedReader):
            self.file = data.name.encode()
            self.data = data.read()
        else:
            self.data = data

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
            symbol = END_OF_FILE if self.lineno() else END_OF_LINE
            return self.lineno(), 0, symbol

        # Determine what we are most likely parsing.
        if char_isnamechar(self.head()) and not char_isdigitchar(self.head()):
            nexter = self.next_name
        elif char_isdigitchar(self.head()):
            nexter = self.next_numeric
        else:
            nexter = self.next_punc

        token = nexter()
        self.last_symbol = token[2]

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
                if symbol_isclosedb(symbol, self.head()):
                    break
                if symbol_isclosedp(symbol, self.head()):
                    break
                if symbol_iscloseds(symbol, self.head()):
                    break
                if char_istermchar(self.head()):
                    break
                # Names cannot exist in punctuation.
                if not symbol_isvalidpunc(symbol, self.head()):
                    break

            if symbol_isstrsym(symbol):
                self.string_parsing = not self.string_parsing

        return self.lineno(), column, symbol

    def next_string(self):
        """Parse the next string symbol."""

        symbol = bytearray()
        column = (self.read_head - self.last_line_at)

        while True:
            symbol.append(self.head())
            self.advance()

            if symbol_isstrsym(symbol):
                break
            # The next sequence of characters
            # matches the previous symbol.
            if self.lookahead_matchlast():
                break
            if self.head() not in self.last_symbol:
                continue

        return self.lineno(), column, symbol


def char_iscomment(char: bytes | int):
    return char in COMMENT_CHAR


def char_isdigitchar(char: bytes | int):
    return char in DIGIT_CHARS


def char_isdigitsep(char: bytes | int):
    return char in DIGIT_SEP_CHARS


def char_islbrace(char: bytes | int):
    return char in b"{"


def char_islparen(char: bytes | int):
    return char in b"("


def char_islsqbrk(char: bytes | int):
    return char in b"["


def char_isnamechar(char: bytes | int):
    return char in NAME_CHARS


def char_isnewline(char: bytes | int):
    return char in NEWLINE_CHAR


def char_isnoparse(char: bytes | int):
    return char in WHITESPACE


def char_ispuncchar(char: bytes | int):
    return char not in NAME_CHARS


def char_isrbrace(char: bytes | int):
    return char in b"}"


def char_isrparen(char: bytes | int):
    return char in b")"


def char_isrsqbrk(char: bytes | int):
    return char in b"]"


def char_istermchar(char: bytes | int):
    return any([char == tc for tc in TERM_CHARS])


def symbol_isclosedb(symbol: bytearray, next_char: bytes | int):
    return bool(
        symbol
        and char_islbrace(symbol[0])
        and char_isrbrace(next_char))


def symbol_isclosedp(symbol: bytearray, next_char: bytes | int):
    return bool(
        symbol
        and char_islparen(symbol[0])
        and char_isrparen(next_char))


def symbol_iscloseds(symbol: bytearray, next_char: bytes | int):
    return bool(
        symbol
        and char_islsqbrk(symbol[0])
        and char_isrsqbrk(next_char))


def symbol_isname(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] in NAME_CHARS
        and symbol[0] not in DIGIT_CHARS
        and symbol[-1] in NAME_CHARS)


def symbol_isnumeric(symbol: bytearray):
    if not bool(symbol and any([i in DIGIT_CHARS for i in symbol])):
        return False

    for ch in DIGIT_SEP_CHARS[1:]:
        if ch in symbol and b"." in symbol:
            return False

    for ch in symbol:
        if ch not in DIGIT_CHARS + DIGIT_SEP_CHARS:
            return False

    return (symbol.count(b".") < 2)


def symbol_ispunc(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] not in NAME_CHARS
        and symbol[-1] not in NAME_CHARS)


def symbol_isstring(symbol: bytearray):
    for str_symbol in STR_SYMBOLS:
        if str_symbol not in symbol:
            continue
        if str_symbol not in symbol[len(str_symbol):]:
            continue
        if str_symbol not in symbol[:len(str_symbol)]:
            continue
        return True
    return False


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
    isnumeric = symbol_isnumeric(symbol)
    if not isnumeric:
        return False

    if b"." in symbol:
        return not char_isdigitsep(next_char)

    return isnumeric


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
        (6, 1, bytearray(b'c')),
        (6, 2, bytearray(b':')),
        (6, 4, bytearray(b'str')),
        (6, 8, bytearray(b'=')),
        (6, 10, bytearray(b"\'\'\'d%\'-\'`\'\'\'")),
        (6, 22, bytearray(b';')),
        (7, 1, bytearray(b'x')),
        (7, 2, bytearray(b'++')),
        (7, 4, bytearray(b';')),
        (7, 6, bytearray(b's')),
        (7, 7, bytearray(b':')),
        (7, 9, bytearray(b'flt')),
        (7, 13, bytearray(b'=')),
        (7, 15, bytearray(b'49.9')),
        (7, 19, bytearray(b'.')),
        (7, 20, bytearray(b'3')),
        (7, 21, bytearray(b';')),
        (10, 0, bytearray(b'EOF'))
    ]

    with open("grammar/parse_test.vxn", "rb") as fd:
        for symbol in BasicSymbolParser(fd):
            symbols_parsed.append(symbol)

    for st, sp in zip(symbols_tested, symbols_parsed):
        print(st, sp, sep="  \t\t")
        assert st == sp, f"Received invalid token {sp!r}."

    return 0


if __name__ == "__main__":
    exit(main())
