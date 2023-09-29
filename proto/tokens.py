from .symbols import Symbol, Column, Lineno, SymbolParser
from .symbols import END_OF_FILE, END_OF_LINE


class Token:
    symbol: Symbol
    lineno: Lineno
    column: Column
    file:   bytes | None

    def __init__(
        self,
        symbol: Symbol,
        lineno: Lineno,
        column: Column,
        file: bytes | None = None):
        """Initialize a `Token` object."""

        self.symbol = symbol
        self.lineno = lineno
        self.column = column
        self.file   = file

    def __repr__(self):
        return (
            f"{self.__class__.__name__}"
            f"[{self.symbol.decode()!r}]"
            f"@(lineno: {self.lineno}, col: {self.column})")


# Tokens must then be separated into different `kind`s
# of tokens.
# 1. name -- the default if unrecognized.
# 2. numerical
# 3. keyword
# 4. punctuation
# 5. ctrl character
class Tokenizer(SymbolParser):
    """Parses a stream of bytes into tokens."""


    def next(self) -> Token:
        """Parse next `Token`."""

        lineno, column, symbol = super().next()
        return Token(symbol, lineno, column)


if __name__ == "__main__":
    import pathlib


    tk = Tokenizer(pathlib.Path("grammar/control.vxn").absolute().read_bytes())
    while not tk.end():
        print(tk.next())
