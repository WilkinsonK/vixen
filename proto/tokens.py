import enum
import io

from .symbols import Symbol, Column, Lineno, SymbolParser


def auto_token(symbol: bytes):
    return (enum.auto(), symbol)


class TokenType(tuple[int, bytes], enum.ReprEnum):
    # Most of the 'types' in this enumerator are
    # too specific for this tokenizer to
    # identifiy on it's own. Most likely, we will
    # keep these definitions here for come parse
    # time.

    Error          = auto_token(b"<error>")
    ErrorUnknown   = auto_token(b"<error:unknown>")

    Kwd            = auto_token(b"<keyword>")
    KwdBreak       = auto_token(b"break")
    KwdContinue    = auto_token(b"continue")
    KwdClass       = auto_token(b"class")
    KwdDelete      = auto_token(b"delete")
    KwdElse        = auto_token(b"else")
    KwdFor         = auto_token(b"for")
    KwdIf          = auto_token(b"if")
    KwdNew         = auto_token(b"new")
    KwdPanic       = auto_token(b"panic")
    KwdProto       = auto_token(b"proto")
    KwdRaise       = auto_token(b"raise")
    KwdReturn      = auto_token(b"return")
    KwdWhile       = auto_token(b"while")
    KwdWith        = auto_token(b"with")

    Name           = auto_token(b"<name>")
    NameOfType     = auto_token(b"<name:type>")

    # '%' is being used to express `a consecutive
    # series of numerical characters`. This means
    # that we can expect an integer-like sequence
    # in the token at that position.
    NameOfTypeInt  = auto_token(br"int%")
    NameOfTypeChar = auto_token(b"char")
    NameOfTypeStr  = auto_token(b"str")
    NameOfTypeFlt  = auto_token(b"flt")
    NameOfTypeDbl  = auto_token(b"dbl")
    NameOfVar      = auto_token(b"<name:variable>")

    Num            = auto_token(b"<numeric>")
    NumFlt         = auto_token(br"%.%")
    NumHex         = auto_token(br"0x%")
    NumInt         = auto_token(br"%")
    NumOct         = auto_token(br"0o%")

    Oper           = auto_token(b"<operation>")
    OperAdd        = auto_token(b"+")
    OperAddEq      = auto_token(b"+=")
    OperAddressOf  = auto_token(b"&")
    OperAssign     = auto_token(b"=")
    OperBtAnd      = auto_token(b"&")
    OperBtOr       = auto_token(b"|")
    OperBtXor      = auto_token(b"^")
    OperDecrement  = auto_token(b"--")
    OperDelete     = auto_token(b"~")
    OperDivide     = auto_token(b"/")
    OperDivFloor   = auto_token(b"//")
    OperDot        = auto_token(b".")
    OperIncrement  = auto_token(b"++")
    OperLgAnd      = auto_token(b"&&")
    OperLgNot      = auto_token(b"!")
    OperLgOr       = auto_token(b"||")
    OperLgGt       = auto_token(b">")
    OperLgGte      = auto_token(b">=")
    OperLgLt       = auto_token(b"<")
    OperLgLte      = auto_token(b"<=")
    OperPointer    = auto_token(b"*")
    OperPower      = auto_token(b"**")
    OperRemainder  = auto_token(br"%%")
    OperMultiply   = auto_token(b"*")
    OperStamp      = auto_token(b"@")
    OperSubtract   = auto_token(b"-")
    OperSubtractEq = auto_token(b"-=")

    Punc           = auto_token(b"<punctuation>")
    PuncColon      = auto_token(b":")
    PuncLBrace     = auto_token(b"{")
    PuncLBracket   = auto_token(b"[")
    PuncLParen     = auto_token(b"(")
    PuncRBrace     = auto_token(b"}")
    PuncRBracket   = auto_token(b"]")
    PuncRParen     = auto_token(b")")
    PuncTerminator = auto_token(b";")

    CTRLChar       = auto_token(b"<ctrl_character>")
    CTRLCharEOF    = auto_token(b"EOF")
    CTRLCharEOL    = auto_token(b"EOL")


class Token:
    symbol: Symbol
    type_:  TokenType
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

        self.type_ = TokenType.Name

    def __repr__(self):

        if self.file:
            location = (
                f"("
                    f"lineno: {self.lineno}, "
                    f"col: {self.column}, "
                    f"file: {self.file.decode()!r}"
                f")")
        else:
            location = f"(lineno: {self.lineno}, col: {self.column})"

        return (
            f"{self.type_.name}{self.__class__.__name__}"
            f"[{self.symbol.decode()!r}]"
            f"@{location}")

    def set_type(self, type_: TokenType):
        """Set the token type."""

        self.type_ = type_


class Tokenizer(SymbolParser):
    """Parses a stream of bytes into tokens."""

    file: bytes | None

    def __init__(self, data: bytes | str | io.TextIOWrapper):
        self.file = None
        if isinstance(data, io.TextIOWrapper):
            self.file = data.name.encode()
            data = data.read()
        super().__init__(data)

    def next(self) -> Token:
        """Parse next `Token`."""

        lineno, column, symbol = super().next()
        return Token(symbol, lineno, column, self.file)


if __name__ == "__main__":
    with open("grammar/control.vxn") as fd:
        tk = Tokenizer(fd)
        while not tk.end():
            print(tk.next())
