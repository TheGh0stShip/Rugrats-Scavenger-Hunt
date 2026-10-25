"""``n64dk`` command line: subcommands register themselves from ``n64dk.commands``."""
from __future__ import annotations

import argparse
import importlib
import json
import pkgutil
import sys
from typing import Callable

COMMANDS: dict[str, tuple[Callable, Callable, str]] = {}


def command(name: str, help: str):
    """Decorator pair: ``@command(name, help)`` on a function ``(args) -> int | dict`` with
    an ``arguments(parser)`` attribute set via ``.arguments``."""
    def wrap(fn):
        COMMANDS[name] = (fn, None, help)
        return fn
    return wrap


def load_commands() -> None:
    from . import commands
    for mod in pkgutil.iter_modules(commands.__path__):
        importlib.import_module(f"{commands.__name__}.{mod.name}")


def emit(result, fmt: str) -> None:
    if result is None or isinstance(result, int):
        return
    if fmt == "json":
        json.dump(result, sys.stdout, indent=1, default=str)
        sys.stdout.write("\n")
    elif isinstance(result, dict):
        for k, v in result.items():
            print(f"{k}: {v}")
    else:
        print(result)


def main(argv: list[str] | None = None) -> int:
    load_commands()
    parser = argparse.ArgumentParser(prog="n64dk", description=__doc__)
    parser.add_argument("--format", choices=("text", "json"), default="text")
    sub = parser.add_subparsers(dest="command", required=True)
    for name, (fn, _unused, help_text) in sorted(COMMANDS.items()):
        p = sub.add_parser(name, help=help_text)
        arguments = getattr(fn, "arguments", None)
        if arguments:
            arguments(p)
        p.set_defaults(_fn=fn)
    args = parser.parse_args(argv)
    result = args._fn(args)
    emit(result, args.format)
    return result if isinstance(result, int) else 0


if __name__ == "__main__":
    sys.exit(main())
