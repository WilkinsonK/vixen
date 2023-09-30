import enum
import io
import typing

from .symbols import Symbol, Column, Lineno, SymbolParser
from .symbols import symbol_ispunc, symbol_isname, symbol_isnumeric

TTErrorMapping:    typing.Mapping[bytes, int] = {}
TTKwdMapping:      typing.Mapping[bytes, int] = {}
TTNameMapping:     typing.Mapping[bytes, int] = {}
TTNumericsMapping: typing.Mapping[bytes, int] = {}
TTOperMapping:     typing.Mapping[bytes, int] = {}
TTPuncMapping:     typing.Mapping[bytes, int] = {}
TTCTRLMapping:     typing.Mapping[bytes, int] = {}


def auto_token(symbol: bytes, *, parent: tuple[int, bytes] | None = None): #type: ignore
    """
    Maps token IDs to their respective symbol.
    """

    pair = (enum.auto(), symbol)

    if parent:
        if parent[1] == b"<keyword>":
            TTKwdMapping[symbol] = pair[0]
        elif parent[1] == b"<name:type>":
            TTNameMapping[symbol] = pair[0]
        elif parent[1] == b"<numeric>":
            TTNumericsMapping[symbol] = pair[0]
        elif parent[1] == b"<operation>":
            TTOperMapping[symbol] = pair[0]
        elif parent[1] == b"<punctuation>":
            TTPuncMapping[symbol] = pair[0]
        elif parent[1] == b"<ctrl_character>":
            TTCTRLMapping[symbol] = pair[0]

    return pair


class TokenType(tuple[int, bytes], enum.ReprEnum):
    # Most of the 'types' in this enumerator are
    # too specific for this tokenizer to
    # identifiy on it's own. Most likely, we will
    # keep these definitions here for come parse
    # time.

    Error          = auto_token(b"<error>")
    ErrorUnknown   = auto_token(b"<error:unknown>")

    Kwd            = auto_token(b"<keyword>")
    KwdBreak       = auto_token(b"break", parent=Kwd)
    KwdContinue    = auto_token(b"continue", parent=Kwd)
    KwdClass       = auto_token(b"class", parent=Kwd)
    KwdDelete      = auto_token(b"delete", parent=Kwd)
    KwdElse        = auto_token(b"else", parent=Kwd)
    KwdFor         = auto_token(b"for", parent=Kwd)
    KwdIf          = auto_token(b"if", parent=Kwd)
    KwdImport      = auto_token(b"import", parent=Kwd)
    KwdNew         = auto_token(b"new", parent=Kwd)
    KwdPanic       = auto_token(b"panic", parent=Kwd)
    KwdProto       = auto_token(b"proto", parent=Kwd)
    KwdRaise       = auto_token(b"raise", parent=Kwd)
    KwdReturn      = auto_token(b"return", parent=Kwd)
    KwdWhile       = auto_token(b"while", parent=Kwd)
    KwdWith        = auto_token(b"with", parent=Kwd)

    Name           = auto_token(b"<name>")
    NameOfType     = auto_token(b"<name:type>")

    # '%' is being used to express `a consecutive
    # series of numerical characters`. This means
    # that we can expect an integer-like sequence
    # in the token at that position.
    NameOfTypeInt  = auto_token(br"int%", parent=NameOfType)
    NameOfTypeChar = auto_token(b"char", parent=NameOfType)
    NameOfTypeStr  = auto_token(b"str", parent=NameOfType)
    NameOfTypeFlt  = auto_token(b"flt", parent=NameOfType)
    NameOfTypeDbl  = auto_token(b"dbl", parent=NameOfType)
    NameOfVar      = auto_token(b"<name:variable>")

    Num            = auto_token(b"<numeric>")
    NumFlt         = auto_token(br"%.%", parent=Num)
    NumHex         = auto_token(br"0x%", parent=Num)
    NumInt         = auto_token(br"%", parent=Num)
    NumOct         = auto_token(br"0o%", parent=Num)

    Oper           = auto_token(b"<operation>")
    OperAdd        = auto_token(b"+", parent=Oper)
    OperAddEq      = auto_token(b"+=", parent=Oper)
    OperAddressOf  = auto_token(b"&", parent=Oper)
    OperAssign     = auto_token(b"=", parent=Oper)
    OperBtAnd      = auto_token(b"&", parent=Oper)
    OperBtOr       = auto_token(b"|", parent=Oper)
    OperBtXor      = auto_token(b"^", parent=Oper)
    OperDecrement  = auto_token(b"--", parent=Oper)
    OperDelete     = auto_token(b"~", parent=Oper)
    OperDivide     = auto_token(b"/", parent=Oper)
    OperDivFloor   = auto_token(b"//", parent=Oper)
    OperDot        = auto_token(b".", parent=Oper)
    OperIncrement  = auto_token(b"++", parent=Oper)
    OperLgAnd      = auto_token(b"&&", parent=Oper)
    OperLgNot      = auto_token(b"!", parent=Oper)
    OperLgOr       = auto_token(b"||", parent=Oper)
    OperLgGt       = auto_token(b">", parent=Oper)
    OperLgGte      = auto_token(b">=", parent=Oper)
    OperLgLt       = auto_token(b"<", parent=Oper)
    OperLgLte      = auto_token(b"<=", parent=Oper)
    OperPointer    = auto_token(b"*", parent=Oper)
    OperPower      = auto_token(b"**", parent=Oper)
    OperRemainder  = auto_token(br"%%", parent=Oper)
    OperMultiply   = auto_token(b"*", parent=Oper)
    OperStamp      = auto_token(b"@", parent=Oper)
    OperSubtract   = auto_token(b"-", parent=Oper)
    OperSubtractEq = auto_token(b"-=", parent=Oper)

    Punc           = auto_token(b"<punctuation>")
    PuncColon      = auto_token(b":", parent=Punc)
    PuncLBrace     = auto_token(b"{", parent=Punc)
    PuncLBracket   = auto_token(b"[", parent=Punc)
    PuncLParen     = auto_token(b"(", parent=Punc)
    PuncRBrace     = auto_token(b"}", parent=Punc)
    PuncRBracket   = auto_token(b"]", parent=Punc)
    PuncRParen     = auto_token(b")", parent=Punc)
    PuncTerminator = auto_token(b";", parent=Punc)

    Str            = auto_token(b"<string>")
    StrSingleBkt   = auto_token(b"`%`", parent=Str)
    StrSingleDbl   = auto_token(b"\"%\"", parent=Str)
    StrSingleSgl   = auto_token(b"'%'", parent=Str)
    StrTripleBkt   = auto_token(b"```%```", parent=Str)
    StrTripleDbl   = auto_token(b"\"\"\"%\"\"\"", parent=Str)
    StrTripleSgl   = auto_token(b"'''%'''", parent=Str)

    CTRLChar       = auto_token(b"<ctrl_character>")
    CTRLCharEOF    = auto_token(b"EOF", parent=CTRLChar)
    CTRLCharEOL    = auto_token(b"EOL", parent=CTRLChar)


class Token:
    symbol: Symbol
    type_:  TokenType
    lineno: Lineno
    column: Column
    file:   bytes | None

    def __init__(
        self,
        lineno: Lineno,
        column: Column,
        symbol: Symbol,
        file: bytes | None = None):
        """Initialize a `Token` object."""

        self.symbol = symbol
        self.lineno = lineno
        self.column = column
        self.file   = file

        if symbol_isnumeric(symbol):
            self.set_type(TokenType.Num)
        elif symbol_ispunc(symbol):
            self.set_type(TokenType.Punc)
        elif symbol_isname(symbol):
            self.set_type(TokenType.Name)
        else:
            self.set_type(TokenType.ErrorUnknown)

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

    def get_type(self):
        """Get the token type."""

        return self.type_

    def set_type(self, type_: TokenType):
        """Set the token type."""

        self.type_ = type_


class Tokenizer(SymbolParser):
    """Parses a stream of bytes into tokens."""

    def next(self) -> Token:
        """Parse next `Token`."""

        return Token(*super().next(), self.file)


if __name__ == "__main__":
    with open("grammar/control.vxn", "rb") as fd:
        tk = Tokenizer(fd)
        while not tk.end():
            print(tk.next())
