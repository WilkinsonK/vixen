#!/bin/python3
import argparse as ap
import os
import sys

from .parser import TreeParser
from .tokens import BasicLexer


class VixenCliNamespace(ap.Namespace):
    file: str | None
    cinput: str | None


def panic(message: str, prog: ap.ArgumentParser | None = None):
    print_error(message)
    if prog:
        prog.print_help()
    quit(1)


def print_error(message: str):
    print(
        f"{os.path.relpath(__file__)}: error: {message}",
        file=sys.stderr)


def main():
    prog = ap.ArgumentParser(
        os.path.relpath(__file__),
        description="Vixen Programming Language.")

    prog.add_argument("file", nargs="?")
    prog.add_argument("-c",
        type=lambda i: i.encode(),
        dest="cinput",
        help="interperate input.")

    args = prog.parse_args(namespace=VixenCliNamespace())
    if not args.file and not args.cinput:
        panic("no input was provided.", prog)

    if args.cinput:
        lexer = BasicLexer(args.cinput)
    else:
        lexer = BasicLexer(file=args.file)

    tp = TreeParser(lexer)
    tp.parse()
    for node in tp.nodes:
        print(repr(node.token))


if __name__ == "__main__":
    exit(main())
