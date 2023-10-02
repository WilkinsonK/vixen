import abc
import typing

from .symbols import Symbol
from .tokens import BasicLexer, Lexer, Token, TokenType


class TreeNode(typing.Protocol):
    """Abstract Syntax Tree (AST) node."""

    @property
    def token(self) -> Token:
        """Underlying token."""

    @property
    def ttype(self) -> TokenType:
        """Underlying token type."""

    @property
    def symbol(self) -> Symbol:
        """Underlying parsed value."""

    @abc.abstractmethod
    def add_child(self, node: typing.Self) -> None:
        """Add child node to this node."""

    @abc.abstractmethod
    def get_children(self, node: typing.Self) -> typing.Sequence[typing.Self]:
        """Get the child nodes of this node."""

    @abc.abstractmethod
    def get_parent(self) -> typing.Self:
        """Get the parent node to this node."""

    @abc.abstractmethod
    def set_parent(self, node: typing.Self) -> None:
        """Set the parent node of this node."""


class BasicTreeNode(TreeNode):
    parent:   typing.Self | None
    children: list[typing.Self]
    tk:       Token

    def __init__(self, token: Token, parent: typing.Self | None = None):
        self.parent = parent
        self.children = []
        self.tk = token

    @property
    def token(self):
        return self.tk

    @property
    def ttype(self):
        return self.tk.ttype

    @property
    def symbol(self):
        return self.tk.symbol

    def add_child(self, node: typing.Self):
        self.children.append(node)

    def get_children(self, node: typing.Self):
        return self.children

    def get_parent(self):
        return self.parent

    def set_parent(self, node: typing.Self):
        self.parent = node


class TreeParser:
    nodes:         typing.Sequence[TreeNode]
    nodes_history: tuple[TreeNode, TreeNode, TreeNode]
    lexer:         Lexer

    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.nodes = []

        # Prepopulating token history with
        # junk nodes.
        dummy_token = Token(-1, -1, bytearray())
        self.nodes_history = tuple([BasicTreeNode(dummy_token)]*3) #type: ignore[assignment]

    @property
    def current(self):
        """The node currently being parsed."""

        return self.nodes_history[-1]

    @property
    def last(self):
        """
        The last node parsed, right before the
        `previous` node.
        """

        return self.nodes_history[-3]

    @property
    def previous(self):
        """
        The last node to be parsed, right before
        the `current` node.
        """

        return self.nodes_history[-2]

    def done(self):
        """
        Parsing has reached end of lexer tokens.
        """

        return self.lexer.end()

    def parse(self):
        """
        Parse the nodes into from lexer into
        tree.
        """

        while not self.done():
            self.update()
            # TODO: implment parsing.
            print(repr(self.current.token))

    def update(self):
        """
        Gets the next `TreeNode` from the lexer
        and updates the node history with that
        node.
        """

        node = BasicTreeNode(self.lexer.next())
        self.nodes_history = (*self.nodes_history[-2:], node)


if __name__ == "__main__":
    with open("tests/test_parser.vxn", "rb") as fd:
        tp = TreeParser(BasicLexer(fd))
        tp.parse()
