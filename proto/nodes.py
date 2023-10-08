import typing

from .tokens import Token, TokenType


class TreeNode(typing.Protocol):
    """
    A parsed expression from a token, or series
    of tokens in the form of child nodes.
    """

    def reduce(self) -> typing.Mapping:
        """Digest this node into a mapping."""


class ProgramNode(TreeNode):
    """Top most node in an AST."""

    body: list[TreeNode]

    def __init__(self):
        self.body = []

    def reduce(self) -> typing.Mapping:
        return {"program": [node.reduce() for node in self.body]}
    
    def add(self, node: TreeNode):
        """Adds a node to this program body."""

        self.body.append(node)


class StatementNode(TreeNode):
    """
    A phrase composed of multiple nodes.
    """

    kind: TokenType

    def __init__(self, kind: TokenType) -> None:
        self.kind = kind

    def reduce(self) -> typing.Mapping:
        return {"kind": self.kind.name}


class ExpressionNode(StatementNode):
    """
    A node which depends on two or more values
    (e.g. "1 + 2" or "x = y").
    """


class BinaryExpressionNode(ExpressionNode):
    """
    A node which depends on three child nodes.
    A left, an operator, and a right.
    """

    operator: Token
    left:     ExpressionNode
    right:    ExpressionNode

    def __init__(
        self,
        operator: Token,
        left: ExpressionNode,
        right: ExpressionNode):

        super().__init__(operator.ttype)
        self.operator = operator
        self.left     = left
        self.right    = right

    def __repr__(self):
        info = (
            "["
            f"left: {self.left}, op: {self.operator!r}, right: {self.right}"
            "]"
        )
        return f"{self.__class__.__name__}{info}"

    def reduce(self) -> typing.Mapping:
        return super().reduce() | {
            "operator": self.operator.symbol,
            "lineno": self.operator.lineno,
            "column": self.operator.column,
            "left": self.left.reduce(),
            "right": self.right.reduce()
        }


class LiteralNode(ExpressionNode):
    """A literal value."""

    value: Token

    def __init__(self, value: Token) -> None:
        super().__init__(value.ttype)
        self.value = value

    def __repr__(self):
        return f"{self.__class__.__name__}[value: {self.value!r}]"

    def reduce(self) -> typing.Mapping:
        return super().reduce() | {
            "kind": self.kind.name,
            "value": self.value.symbol,
            "lineno": self.value.lineno,
            "column": self.value.column
        }

    @property
    def ttype(self):
        return self.value.ttype


class LiteralIdentNode(LiteralNode):
    """
    A literal expression which identifies another
    instance.
    """


class LiteralNumericNode(LiteralNode):
    """A literal numerical value."""


class LiteralFltNode(LiteralNumericNode):
    """A literal floating number value."""


class LiteralIntNode(LiteralNumericNode):
    """
    A literal numerical value which digests to an
    int.
    """
