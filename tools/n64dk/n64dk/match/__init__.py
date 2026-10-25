"""Matching engine: toolchain profiles, build pipelines, relocated ROM comparison with
classified instruction diffs, configuration sweeps, manifest batches and a permuter driver."""
from .compare import Result, compare_object, relocate
from .diff import Report
from .engine import Outcome, Target, batch, manifest_row, match_source, sweep
from .manifest import Unit, parse as parse_manifest
from .pipeline import PIPELINES, BuildError, BuildSpec, build, split_flags
from .symbols import SymbolMap
from .toolchain import PROFILES, Config, load_config

__all__ = ["Result", "compare_object", "relocate", "Report", "Outcome", "Target", "batch",
           "manifest_row", "match_source", "sweep", "Unit", "parse_manifest", "PIPELINES",
           "BuildError", "BuildSpec", "build", "split_flags", "SymbolMap", "PROFILES", "Config",
           "load_config"]
