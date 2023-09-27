import os
import string

COMMENT_CHAR = b"#"
DIGIT_CHARS = string.digits.encode()
DIGIT_SEP_CHARS = b"."
END_OF_FILE = bytearray(b"EOF")
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


class SymbolParser:
    """
    Parses a buffer of data into symbols that are
    usable for token parsing.
    """

    data:           bytes
    current_char:   int
    read_head:      int
    string_parsing: bool
    last_symbol:    bytearray

    def __init__(self, data: bytes | str):

        if isinstance(data, str):
            data = data.encode()

        self.read_head = 0
        self.data = data
        self.last_symbol = bytearray()
        self.string_parsing = False

    def advance(self):
        """Move the read head forward."""

        self.read_head += 1

    def end(self):
        """
        Read head is at the end of data stream or
        not.
        """

        return self.read_head >= len(self.data)

    def head(self):
        """Character at read head."""

        # Fixes a bug where iterating to `the end`
        # does not always return the last symbol.
        # In particular, when the final symbol is
        # 1 char long.
        if self.read_head >= len(self.data):
            return self.data[-1]
        return self.data[self.read_head]

    def isnoparse(self):
        """
        Read head is a character not meant to be
        parsed.
        """

        return self.head() in WHITESPACE

    def lookahead(self, head: int):
        """
        Get slice of bytes of `head` length of
        data stream relative to read head.
        """

        return self.data[self.read_head:self.read_head+head]

    def next(self):
        """Parse next symbol."""

        symbol = bytearray()

        if not self.string_parsing:
            while self.isnoparse() and not self.end():
                self.advance()

            while _char_iscomment(self.head()):
                while not _char_isnewline(self.head()) and not self.end():
                    self.advance()
                # Second pass ensures additional whitespace
                # after a comment is eliminated.
                while self.isnoparse() and not self.end():
                    self.advance()

            if self.end():
                return END_OF_FILE

        while True:
            if self.isnoparse() and not self.string_parsing:
                break

            if not self.string_parsing:
                # Punctuation cannot exist in a name.
                # Unless said name is numeric, then
                # '.' is accepted.
                if not self.symbol_isvalidname(symbol):
                    if not self.symbol_isnumeric(symbol):
                        break
                # Names cannot exist in punctuation.
                if not self.symbol_isvalidpunc(symbol):
                    break
                # Terminator char must start new
                # sequence.
                if self.symbol_terminated(symbol):
                    break

            # Indescriminate string parsing.
            elif self.head() in self.last_symbol:
                # String termination must match
                # string initiation.
                if self.lookahead(len(self.last_symbol)) == self.last_symbol:
                    break

            # String termination should terminate
            # string parsing.
            elif symbol in STR_SYMBOLS:
                break

            symbol.append(self.head())

            if self.end():
                break
            self.advance()

        self.last_symbol = symbol
        if symbol in STR_SYMBOLS:
            self.string_parsing = not self.string_parsing

        return symbol

    def symbol_isnumeric(self, symbol: bytearray):
        """
        Symbol and read head will parse a numeric
        name.
        """

        return (
            _symbol_isnumeric(symbol)
            and _char_isdigitsep(self.head())
            and (symbol.count(b".") + 1) < 2)

    def symbol_isvalidname(self, symbol: bytearray):
        """
        Symbol and read head will parse a valid
        name.
        """

        return not (_symbol_isname(symbol) and _char_ispuncchar(self.head()))

    def symbol_isvalidpunc(self, symbol: bytearray):
        """
        Symbol and read head will parse valid
        punctuation.
        """

        return not (_symbol_ispunc(symbol) and _char_isnamechar(self.head()))

    def symbol_terminated(self, symbol: bytearray):
        """
        Symbol has been terminated by termination
        char in read head.
        """

        return (
            _symbol_ispunc(symbol)
            and (
                _char_istermchar(self.head())
                or _char_iscomment(self.head())
            ))


def _char_iscomment(char: bytes | int):
    return char in COMMENT_CHAR


def _char_isdigitsep(char: bytes | int):
    return char in DIGIT_SEP_CHARS


def _char_isnamechar(char: bytes | int):
    return char in NAME_CHARS


def _char_isnewline(char: bytes | int):
    return char in NEWLINE_CHAR


def _char_ispuncchar(char: bytes | int):
    return char not in NAME_CHARS


def _char_istermchar(char: bytes | int):
    return char in TERM_CHARS


def _symbol_isname(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] in NAME_CHARS
        and symbol[-1] in NAME_CHARS)


def _symbol_isnumeric(symbol: bytearray):
    return all([i in DIGIT_CHARS + DIGIT_SEP_CHARS for i in symbol])


def _symbol_ispunc(symbol: bytearray):
    return bool(
        symbol
        and symbol[0] not in NAME_CHARS
        and symbol[-1] not in NAME_CHARS)


def main():
    with open("grammar/parse_test.vxn", "rb") as fd:
        reader = SymbolParser(fd.read())

    while not reader.end():
        print("SYMBOL:", reader.next())

    return 0


if __name__ == "__main__":
    exit(main())
