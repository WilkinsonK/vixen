import typing

from .symbols import SymbolParser
from .tokens import BasicLexer, Lexer, Token


class TreeNode:
    parent:   typing.Self | None
    children: typing.Sequence[typing.Self]
    tk:       Token

    def __init__(self, token: Token, parent: typing.Self | None = None):
        self.parent = parent
        self.children = []
        self.tk = token

    @property
    def token(self):
        """Underlying token."""

        return self.tk

    @property
    def ttype(self):
        """Underlying token type."""

        return self.tk.ttype


class TreeParser:
    nodes: typing.Sequence[TreeNode]
    lexer: Lexer

    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.nodes = []

    def parse(self):
        """
        Parse the nodes into from lexer into
        tree.
        """

        for tk in self.lexer:
            self.nodes.append(TreeNode(tk))


if __name__ == "__main__":
    with open("tests/test_parser.vxn", "rb") as fd:
        tp = TreeParser(BasicLexer(fd))
        tp.parse()
