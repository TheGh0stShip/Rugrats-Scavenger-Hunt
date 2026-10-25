# Vendored N64 analysis and matching package

This directory contains the Python source used by the repository's Make
targets. It is intentionally stored with the project so verification does not
depend on a sibling checkout or a machine-specific path.

The package requires Python 3.11 or newer and `rabbitizer`; install the root
`requirements.txt` into a virtual environment. Run it from the repository root
with:

```sh
PYTHONPATH=tools/n64dk python3 -m n64dk --help
```

Historical proprietary compilers are not part of this package. They are loaded
from the ignored `.toolchain/` directory described in the root README.
