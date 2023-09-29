#!/bin/python3
"""Basic ASM translation layer."""

import enum
import typing


AsmOperation = typing.Callable[["Registers", bytes], bytes]


class Registers(enum.Enum, metaclass=enum.EnumType):
    """
    Represents the system registers for a
    specific OS and architecture.
    """


class RegistersLinux_x86_64(Registers):
    rax = enum.auto()
    rdi = enum.auto()


def cmdglobal(name: bytes):
    return b"global %s" % (name,)


def cmdsegment(name: bytes):
    return b"segment %s" % (name,)


def mdefine(name: bytes, data: bytes):
    return b"%%define %s %s" % (name, data)


def operation(op: bytes, reg: Registers, data: bytes):
    return b"%s %s, %s" % (op, reg.name.encode(), data)


def opmov(reg: Registers, data: bytes):
    return operation(b"mov", reg, data)


if __name__ == "__main__":
    print(opmov(RegistersLinux_x86_64.rax, b"42"))
