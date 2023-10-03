import abc
import typing

from .symbols import Symbol
from .tokens import BasicLexer, Lexer, Token, TokenType

TreeNodeMapping: typing.Mapping[str, type["TreeNode"]] = {}


def register_node_type(cls: type["TreeNode"]) -> type["TreeNode"]:
    """Maps node types for parse lookup."""

    TreeNodeMapping[cls.__name__] = cls #type: ignore[index]
    return cls


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

    @classmethod
    @abc.abstractmethod
    def token_isviable(cls, token: Token) -> bool:
        """
        Given token is an eligible candidate for
        this node type.
        """


class BasicNode(TreeNode):
    parent:   TreeNode | None
    children: list[TreeNode]
    tk:       Token

    # Eligible token types.
    tk_viable_types: tuple[TokenType, ...] = ()

    def __init__(self, token: Token, parent: TreeNode | None = None):
        self.parent = parent
        self.children = []
        self.tk = token

    def __repr__(self):
        return f"{self.__class__.__name__}({self.token!r})"

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

    @classmethod
    def token_isviable(cls, token: Token):
        return token.ttype in cls.tk_viable_types


class ErrorNode(BasicNode):
    """
    Represents a node where there was an error in
    parsing.
    """


class ValueNode(BasicNode):
    """
    A node which requires no arguments, but
    instead represents a value.
    """


class NumericNode(ValueNode):
    """A node representing a numeric value."""



@register_node_type
class NumericFltNode(NumericNode):
    """
    A node representing a floating point
    numerical value.
    """


@register_node_type
class NumericIntNode(NumericNode):
    """A node representing an integer value."""

    tk_viable_types = (
        TokenType.NumBin,
        TokenType.NumInt,
        TokenType.NumHex,
        TokenType.NumOct)


@register_node_type
class UnaryNode(BasicNode):
    """
    Represents an operartion which takes one
    argument.

    The argument is consumed into this node as a
    child node.
    """

    tk_viable_types = (
        TokenType.OperBtAnd,
        TokenType.OperStamp
    )

    # &x; // address of name.
    # *x; // pointer dereference.
    def __init__(self, token: Token, parent: TreeNode | None = None):
        super().__init__(token, parent)
        self.children = [dummy_node()]

    @property
    def child(self):
        return self.children[0]


@register_node_type
class BinaryOperNode(BasicNode):
    """
    Represents an operation which takes two
    arguments.

    Arguments are consumed in this node as child
    nodes; a left and a right.
    """

    tk_viable_types = (
        TokenType.OperDivide,
        TokenType.OperDivFloor,
        TokenType.OperMinus,
        TokenType.OperPlus,
        TokenType.OperStar
    )

    # x + y; // Arithmetic operations.
    # x = y; // Assignment operations.
    # x : int; // name declaration.
    def __init__(self, token: Token, parent: TreeNode | None = None):
        super().__init__(token, parent)
        self.children = [dummy_node()]*2

    @property
    def left(self):
        return self.children[0]

    @property
    def right(self):
        return self.children[1]


@register_node_type
class TernaryOperNode(BasicNode):
    """
    Represents an operation which takes three
    arguments.

    Arguments are consumed in this node as child
    nodes; a left, a right and a center.
    """

    # """some_junk"""; // String initializations.
    # (x, y); // Tuple initialization.
    # (x + (y * 2)); // Complex arithmetic operations.
    def __init__(self, token: Token, parent: TreeNode | None = None):
        super().__init__(token, parent)
        self.children = [dummy_node()]*3

    @property
    def left(self):
        return self.children[0]

    @property
    def center(self):
        return self.children[1]

    @property
    def right(self):
        return self.children[2]


class TreeParser:
    nodes:            typing.Sequence[TreeNode]
    nodes_history:    tuple[TreeNode, TreeNode, TreeNode]
    lexer:            Lexer

    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.nodes = []

        # Prepopulating token history with
        # junk nodes.
        self.nodes_history = tuple([dummy_node()]*3) #type: ignore[assignment]

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

            if not isinstance(self.current, ErrorNode):
                # TODO: implment parsing.
                print(repr(self.current))
                continue

            token = self.current.token
            ttype = self.current.ttype
            if self.current.ttype is TokenType.NameGeneric:
                raise ParserUnknownNameError(f"Unknown name '{token!s}'.")
            else:
                raise ParserUnsupportedError(f"Unsupported type {ttype.name!r}")

    def update(self):
        """
        Gets the next `TreeNode` from the lexer
        and updates the node history with that
        node.
        """

        tnext = self.lexer.next()
        node  = ErrorNode(tnext)

        for nodet in TreeNodeMapping.values():
            if nodet.token_isviable(tnext):
                node = nodet(tnext)

        self.nodes_history = (*self.nodes_history[-2:], node)


def dummy_node():
    """Creates an instance of an empty node."""

    return BasicNode(Token(-1, -1, bytearray()))


class ParserError(Exception):
    """
    Generic exception for when something goes
    wrong while building AST.
    """


class ParserUnknownNameError(ParserError):
    """
    Raised when a name token is invalid or
    unknown.
    """


class ParserUnsupportedError(ParserError):
    """
    Raised when the token is not a supported type.
    """


if __name__ == "__main__":
    with open("tests/test_parser.vxn", "rb") as fd:
        tp = TreeParser(BasicLexer(fd))
        tp.parse()
