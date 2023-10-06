import typing


class TreeNode(typing.Protocol):
    """
    A parsed expression from a token, or series
    of tokens in the form of child nodes.
    """

    def reduce(self) -> typing.Mapping:
        """Digest this node into a mapping."""

    @classmethod
    def expand(cls, mapping: typing.Mapping) -> typing.Self:
        """
        Create an instance of this node type from
        a mapping.
        """


class ProgramNode(TreeNode):
    """Top most node in an AST."""

    body: tuple[TreeNode, ...]


class StatementNode(TreeNode):
    """
    A phrase composed of multiple nodes.
    """

    kind: str


class ExpressionNode(StatementNode):
    """
    A node which depends on two or more values
    (e.g. "1 + 2" or "x = y").
    """
