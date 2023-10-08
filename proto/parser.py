import typing

from .nodes import (
    BinaryExpressionNode,
    ExpressionNode,
    LiteralNode,
    LiteralFltNode,
    LiteralIdentNode,
    LiteralIntNode,
    ProgramNode,
    StatementNode
)
from .tokens import (
    tokens_isfloat,
    tokens_isgeneric,
    tokens_isinteger,
    BasicLexer,
    Lexer,
    Token,
    TokenType
)

P = typing.ParamSpec("P")
ParseNextProtocol = typing.Callable[P, StatementNode | LiteralNode]


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

    def done(self) -> bool:
        """
        The lexer has been exhausted of all
        available tokens.
        """

    def expect(self, ttype: TokenType) -> None:
        """
        Looks ahead for the next available token,
        validating if the next token is the
        expected `TokenType`.

        This function returns `None` but raises an
        error if the next `TokenType` is not the
        expected.
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


class TreeParser(Parser):

    # Used as the 'token history' much like the
    # ribbon used in olf-fashioned type writers.
    lexer_ribbon: tuple[Token, Token, Token]

    def __init__(self, lexer: Lexer):
        self.lexer        = lexer
        self.lexer_ribbon = (None, lexer.next(), lexer.next())
        self.program      = ProgramNode()

    @property
    def current(self) -> Token:
        return self.lexer_ribbon[1]
    
    @property
    def next(self) -> Token:
        return self.lexer_ribbon[-1]

    @property
    def previous(self) -> Token:
        return self.lexer_ribbon[0]

    def digest(self) -> typing.Mapping:
        return self.program.reduce()

    def done(self):
        return self.current.symbol in (b"EOF", b"EOL")

    def expect(self, ttype: TokenType):
        if self.current.ttype is not ttype:
            got = self.current.ttype
            msg = f"Unexpected token type. Expected {ttype.name!r} got {got.name!r}"
            raise TypeError(msg)

    def update(self):
        self.lexer_ribbon = (*self.lexer_ribbon[-2:], self.lexer.next())

    # Parsing needs to happen at an order of
    # most precedent. (Order of Precedence).
    # Order of precedence dictates in what order
    # of recursion calls are made; where the
    # highest priority is called last, and the
    # lowest first.
    def parse(self):
        while not self.done():
            self.program.add(self.parse_stmt())
            self.update()

    # OoP stmt 0
    def parse_stmt(self) -> StatementNode:
        """
        Parses a `StatementNode` from the
        proceeding tokens.
        """

        # No addtional expression types defined at
        # this moment.
        return self.parse_expr()

    # OoP stmt 1
    def parse_expr(self) -> ExpressionNode:
        """
        Parses an `ExpressionNode` from the
        proceeding tokens.
        """

        # The current lightest expression is the
        # additive expression.
        return self.parse_expr_additive()

    def parse_expr_binary(
        self,
        expected_ops: tuple[TokenType, ...],
        next_parser: ParseNextProtocol | None = None):
        """
        Parses some binary expression from the
        proceeding tokens.
        """
    
        if not next_parser:
            next_parser = self.parse_expr_primative

        left = next_parser()

        while self.current.ttype in expected_ops:
            op = self.current
            self.update()
            right = next_parser()
            left  = BinaryExpressionNode(op, left, right)

        return left

    # OoP stmt 1 expr 0
    def parse_expr_primative(self) -> LiteralNode:
        """
        Parses a primitive node from the next
        lexer tokens.
        """

        tk = self.current
        self.update()
        if tokens_isgeneric(tk):
            return LiteralIdentNode(tk)
        elif tokens_isfloat(tk):
            return LiteralFltNode(tk)
        elif tokens_isinteger(tk):
            return LiteralIntNode(tk)
        elif tk.ttype is TokenType.PuncLParen:
            expr = self.parse_expr()
            self.expect(TokenType.PuncRParen)
            self.update()
            return expr
        else:
            loc = (
                f"(lineno: {tk.lineno}, col: {tk.column})"
            )
            symbol = tk.symbol.decode()
            raise TypeError(f"Unexpected token at {loc}. Got {symbol!r}")

    # OoP stmt 1 expr 1
    def parse_expr_multiplicative(self):
        """
        Parses an additive expression from the
        next lexer tokens. Additive being an
        expression, from left to right produces
        a statement that multiplies, divides or
        gets the remainder from two objects.
        (e.g. "x + y")
        """

        ops = (
            TokenType.OperDivide,
            TokenType.OperDivFloor,
            TokenType.OperModulus,
            TokenType.OperStar,
            TokenType.OperPower
        )

        return self.parse_expr_binary(ops)

    # OoP stmt 1 expr 2
    def parse_expr_additive(self):
        """
        Parses an additive expression from the
        next lexer tokens. Additive being an
        expression, from left to right produces
        a statement that adds or subtracts two
        objects together. (e.g. "x + y")
        """

        # Multiplicative expressions are currently
        # the next lightest after additive. Will
        # try to parse a binary operation first
        # which produces this type of expression
        # for the left child node.
        nexter = self.parse_expr_multiplicative
        ops    = (TokenType.OperPlus, TokenType.OperMinus)

        return self.parse_expr_binary(ops, nexter)


if __name__ == "__main__":
    with open("tests/test_parser.vxn", "rb") as fd:
        tp = TreeParser(BasicLexer(fd))
        tp.parse()
