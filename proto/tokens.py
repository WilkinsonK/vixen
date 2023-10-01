import enum
import typing

from .symbols import Symbol, Column, Lineno, SymbolParser
from .symbols import symbol_isnumeric, symbol_isstring

TokenType = typing.TypeVar("TokenType")
TokenTypeMapping: typing.Mapping[bytes, int] = {}


def auto(symbol: bytes): #type: ignore
    """
    Maps token IDs to their respective symbol.
    """

    value = enum.auto()
    TokenTypeMapping[symbol] = value

    return value


class TokenType(int, enum.ReprEnum):
    # Most of the 'types' in this enumerator are
    # too specific for this tokenizer to
    # identifiy on it's own. Most likely, we will
    # keep these definitions here for come parse
    # time.

    Error          = auto(b"<error>")
    ErrorUnknown   = auto(b"<error:unknown>")

    Kwd            = auto(b"<keyword>")
    KwdBreak       = auto(b"break")
    KwdContinue    = auto(b"continue")
    KwdClass       = auto(b"class")
    KwdDelete      = auto(b"delete")
    KwdElse        = auto(b"else")
    KwdFor         = auto(b"for")
    KwdIf          = auto(b"if")
    KwdImport      = auto(b"import")
    KwdNew         = auto(b"new")
    KwdPanic       = auto(b"panic")
    KwdProto       = auto(b"proto")
    KwdRaise       = auto(b"raise")
    KwdReturn      = auto(b"return")
    KwdWhile       = auto(b"while")
    KwdWith        = auto(b"with")

    Name           = auto(b"<name>")
    NameOfType     = auto(b"<name:type>")

    # '%' is being used to express `a consecutive
    # series of numerical characters`. This means
    # that we can expect an integer-like sequence
    # in the token at that position.
    NameOfTypeInt  = auto(br"int%")
    NameOfTypeChar = auto(b"char")
    NameOfTypeStr  = auto(b"str")
    NameOfTypeFlt  = auto(b"flt")
    NameOfTypeDbl  = auto(b"dbl")
    NameOfVar      = auto(b"<name:variable>")

    Num            = auto(b"<numeric>")
    NumBin         = auto(br"0b%")
    NumFlt         = auto(br"%.%")
    NumHex         = auto(br"0x%")
    NumInt         = auto(br"%")
    NumOct         = auto(br"0o%")

    Oper           = auto(b"<operation>")
    OperAdd        = auto(b"+")
    OperAddEq      = auto(b"+=")
    OperAddressOf  = auto(b"&")
    OperAssign     = auto(b"=")
    OperBtAnd      = auto(b"&")
    OperBtOr       = auto(b"|")
    OperBtXor      = auto(b"^")
    OperDecrement  = auto(b"--")
    OperDelete     = auto(b"~")
    OperDivide     = auto(b"/")
    OperDivFloor   = auto(b"//")
    OperDot        = auto(b".")
    OperIncrement  = auto(b"++")
    OperLgAnd      = auto(b"&&")
    OperLgNot      = auto(b"!")
    OperLgOr       = auto(b"||")
    OperLgGt       = auto(b">")
    OperLgGte      = auto(b">=")
    OperLgLt       = auto(b"<")
    OperLgLte      = auto(b"<=")
    OperPointer    = auto(b"*")
    OperPower      = auto(b"**")
    OperRemainder  = auto(br"%%")
    OperMultiply   = auto(b"*")
    OperStamp      = auto(b"@")
    OperSubtract   = auto(b"-")
    OperSubtractEq = auto(b"-=")

    Punc           = auto(b"<punctuation>")
    PuncColon      = auto(b":")
    PuncLBrace     = auto(b"{")
    PuncLBracket   = auto(b"[")
    PuncLParen     = auto(b"(")
    PuncRBrace     = auto(b"}")
    PuncRBracket   = auto(b"]")
    PuncRParen     = auto(b")")
    PuncTerminator = auto(b";")

    Str            = auto(b"<string>")
    StrSingleBkt   = auto(b"`%`")
    StrSingleDbl   = auto(b"\"%\"")
    StrSingleSgl   = auto(b"'%'")
    StrTripleBkt   = auto(b"```%```")
    StrTripleDbl   = auto(b"\"\"\"%\"\"\"")
    StrTripleSgl   = auto(b"'''%'''")

    CTRLChar       = auto(b"<ctrl_character>")
    CTRLCharEOF    = auto(b"EOF")
    CTRLCharEOL    = auto(b"EOL")


def tokens_find_errunk(symbol: Symbol):
    """
    Dummy function which always returns
    `TokenType.ErrorUnknown`.
    """

    return TokenType.ErrorUnknown


def tokens_find_gentype(symbol: Symbol):
    """
    Identifies the specific `TokenType` of a
    generic symbol.
    """

    try:
        key = TokenTypeMapping[bytes(symbol)]
    except:
        return TokenType.Name

    # Key is `enum.auto` type, not int.
    return TokenType(key.value) #type: ignore


def tokens_find_numtype(symbol: Symbol):
    """
    Identifies the specific `TokenType` of a
    numeric symbol.
    """

    if symbol.count(b"0b") > 0:
        return TokenType.NumBin
    if symbol.count(b".") > 0:
        return TokenType.NumFlt
    if symbol.count(b"0x") > 0:
        return TokenType.NumHex
    if symbol.count(b"0o") > 0:
        return TokenType.NumOct

    return TokenType.NumInt


def tokens_find_strtype(symbol: Symbol):
    """
    Identifies the specific `TokenType` of a
    string symbol.
    """

    if len(symbol) >= 6 and symbol[:3] == symbol[-3:]:
        sample = symbol[:3]
    elif len(symbol) >= 2 and symbol[:1] == symbol[-1:]:
        sample = symbol[:2]
    else:
        return TokenType.ErrorUnknown

    if sample == b"`":
        return TokenType.StrSingleBkt
    if sample == b"\"":
        return TokenType.StrSingleDbl
    if sample == b"'":
        return TokenType.StrSingleSgl
    if sample == b"```":
        return TokenType.StrTripleBkt
    if sample == b"\"\"\"":
        return TokenType.StrTripleDbl
    if sample == b"'''":
        return TokenType.StrTripleSgl

    return TokenType.ErrorUnknown


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
            type_finder = tokens_find_numtype
        elif symbol_isstring(symbol):
            type_finder = tokens_find_strtype
        else:
            type_finder = tokens_find_gentype

        self.set_type(type_finder(symbol))

    def __str__(self):
        return self.symbol.decode()

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
            f"{self.get_type().name}"
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
        for tk in Tokenizer(fd):
            print(repr(tk))
