import typing

from .nodes import ProgramNode
from .symbols import Symbol
from .tokens import BasicLexer, Lexer, Token, TokenType


class Parser(typing.Protocol):
    """
    Parses input tokens from lexical analysis into
    traversable data.
    """

    lexer:   Lexer
    program: ProgramNode

    @property
    def current(self) -> Token:
        """
        Get the current token being observed.
        """

    @property
    def previous(self) -> Token:
        """Get the last token requested."""

    @property
    def next(self) -> Token:
        """Get the next token to be parsed."""

    def digest(self) -> typing.Mapping:
        """
        Consume the parsed tree into a mapping.
        """

    def expect(self, ttype: TokenType):
        """
        Looks ahead for the next available token,
        validating if the next token is the
        expected `TokenType`.
        """

    def parse(self) -> None:
        """
        Parses the tokens provided by the lexer.
        """

    def update(self) -> None:
        """
        Requests the next token from the lexer,
        rotating the token history.
        """

    # Parsing needs to happen at an order of
    # most precedent. (Order of Precedence).
    # Order of precedence dictates in what order
    # of recursion calls are made; where the
    # highest priority is called last, and the
    # lowest first.

    # OoP stmt 0
    def parse_stmt(self):
        pass

    # OoP stmt 1
    def parse_expr(self):
        pass

    # OoP expr 0
    def parse_expr_primative(self):
        pass

    # OoP expr 1
    def parse_expr_additive(self):
        pass

    # OoP expr 2
    def parse_expr_multiplicative(self):
        pass


class TreeParser(Parser):

    def __init__(self, lexer: Lexer):
        self.lexer = lexer


if __name__ == "__main__":
    pass
