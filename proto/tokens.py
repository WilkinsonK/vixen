#!/bin/python3
"""
tokens.py
---

Author: Keenan W. Wilkinson
Date: 1 Oct 2023
---

Provides tools for general purpose lexical analysis where tokens are
parsed first by rules most common to the majority of programming
language structures, and then categorized into `TokenType`s with
additional metadata.

The reasoning behind this module is to ease the responsibility of
parse-time tasks where it should make identifying components of
complex structures simpler, quicker, with pre-categorized elements.
"""

import enum
import typing

from .symbols import Symbol, Column, Lineno, BasicSymbolParser, SymbolParser
from .symbols import symbol_isnumeric

TokenTypeMapping: typing.Mapping[bytes, int] = {}


def auto(symbol: bytes):
    """
    Maps token IDs to their respective symbol.
    """

    value = enum.auto()
    TokenTypeMapping[symbol] = value #type: ignore[index]

    return value


class TokenType(int, enum.ReprEnum):
    # Most of the 'types' in this enumerator are
    # too specific for this tokenizer to
    # identifiy on it's own. Most likely, we will
    # keep these definitions here for come parse
    # time.

    Error          = auto(b"<error>")
    ErrorUnknown   = auto(b"<error:unknown>")
    ErrorBadString = auto(b"<error:bad_string>")

    Kwd            = auto(b"<keyword>")
    KwdAs          = auto(b"as")
    KwdBreak       = auto(b"break")
    KwdCatch       = auto(b"catch")
    KwdContinue    = auto(b"continue")
    KwdConstant    = auto(b"const")
    KwdClass       = auto(b"class")
    KwdDefault     = auto(b"default")
    KwdDelete      = auto(b"delete")
    KwdElse        = auto(b"else")
    KwdFor         = auto(b"for")
    KwdFrom        = auto(b"from")
    KwdFunc        = auto(b"func")
    KwdIf          = auto(b"if")
    KwdImport      = auto(b"import")
    KwdInclude     = auto(b"include")
    KwdNew         = auto(b"new")
    KwdNil         = auto(b"nil")
    KwdNull        = auto(b"null")
    KwdPanic       = auto(b"panic")
    KwdProto       = auto(b"proto")
    KwdRaise       = auto(b"raise")
    KwdReturn      = auto(b"return")
    KwdStatic      = auto(b"static")
    KwdTry         = auto(b"try")
    KwdWhile       = auto(b"while")
    KwdWith        = auto(b"with")

    Name           = auto(b"<name>")
    NameGeneric    = auto(b"<name:generic>")

    Num            = auto(b"<numeric>")
    NumBin         = auto(br"0b%")
    NumFlt         = auto(br"%.%")
    NumHex         = auto(br"0x%")
    NumInt         = auto(br"%")
    NumOct         = auto(br"0o%")

    Oper           = auto(b"<operation>")
    OperAddressOf  = auto(b"&")
    OperAsk        = auto(b"?")
    OperAssign     = auto(b"=")
    OperBtAnd      = auto(b"&")
    OperBtOr       = auto(b"|")
    OperBtXor      = auto(b"^")
    OperDecrement  = auto(b"--")
    OperDelete     = auto(b"~")
    OperDivide     = auto(b"/")
    OperDivFloor   = auto(b"//")
    OperIncrement  = auto(b"++")
    OperLgAnd      = auto(b"&&")
    OperLgNot      = auto(b"!")
    OperLgOr       = auto(b"||")
    OperLgGt       = auto(b">")
    OperLgGte      = auto(b">=")
    OperLgLt       = auto(b"<")
    OperLgLte      = auto(b"<=")
    OperMinus      = auto(b"-")
    OperMinusEq    = auto(b"-=")
    OperModulus    = auto(br"%")
    OperPlus       = auto(b"+")
    OperPlusEq     = auto(b"+=")
    OperPower      = auto(b"**")
    OperPtrAttr    = auto(b"->")
    OperStamp      = auto(b"@")
    OperStar       = auto(b"*")

    Punc           = auto(b"<punctuation>")
    PuncColon      = auto(b":")
    PuncComma      = auto(b",")
    PuncDot        = auto(b".")
    PuncLBrace     = auto(b"{")
    PuncLBracket   = auto(b"[")
    PuncLParen     = auto(b"(")
    PuncRBrace     = auto(b"}")
    PuncRBracket   = auto(b"]")
    PuncRParen     = auto(b")")
    PuncTerminator = auto(b";")

    Str            = auto(b"<string>")
    StrSingleBkt   = auto(b"`")
    StrSingleDbl   = auto(b"\"")
    StrSingleSgl   = auto(b"'")
    StrTripleBkt   = auto(b"```")
    StrTripleDbl   = auto(b"\"\"\"")
    StrTripleSgl   = auto(b"'''")
    StrExpression  = auto(b"<string:expression>")

    CTRLChar       = auto(b"<ctrl_character>")
    CTRLCharEOF    = auto(b"EOF")
    CTRLCharEOL    = auto(b"EOL")


class Token:
    symbol: Symbol
    ttype:  TokenType
    lineno: Lineno #type: ignore
    column: Column #type: ignore
    file:   bytes | None

    def __init__(
        self,
        lineno: Lineno, #type: ignore
        column: Column, #type: ignore
        symbol: Symbol,
        file: bytes | None = None):
        """Initialize a `Token` object."""

        self.symbol = symbol
        self.lineno = lineno
        self.column = column
        self.file   = file

        if symbol_isnumeric(symbol):
            type_finder = tokens_find_numtype
        else:
            type_finder = tokens_find_gentype

        self.ttype = type_finder(symbol)

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
            f"{self.ttype.name}"
            f"[{self.symbol.decode()!r}]"
            f"@{location}")


class Lexer(SymbolParser[Token]):
    """Parses a stream of bytes into tokens."""


class BasicLexer(BasicSymbolParser[Token]):

    # Technically needed only for type analysis.
    def __iter__(self) -> typing.Generator[Token, None, None]:
        while not self.end():
            yield self.next()

    def next(self) -> Token:
        """Parse next `Token`."""

        return Token(*super().next(), self.file) #type: ignore[call-arg]


def tokens_find_errunk(_: Symbol):
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
        return TokenType.NameGeneric

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

    return TokenType.ErrorBadString


def tokens_isfloat(token: Token):
    """Token type is a floating point value."""

    return token.ttype is TokenType.NumFlt


def tokens_isgeneric(token: Token):
    """Token type is an unidentified name."""

    return token.ttype is TokenType.NameGeneric


def tokens_isinteger(token: Token):
    """
    Token type is an integer digestable value.
    """

    types = (
        TokenType.NumBin,
        TokenType.NumHex,
        TokenType.NumInt,
        TokenType.NumOct)
    return token.ttype in types


if __name__ == "__main__":
    with open("grammar/control.vxn", "rb") as fd:
        for tk in BasicLexer(fd):
            print(repr(tk))
