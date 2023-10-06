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
    def parse_primary(self):
        pass

    def parse_additive(self):
        pass

    def parse_multiplicative(self):
        pass


class TreeParser(Parser):
    pass


if __name__ == "__main__":
    pass
