# Pascal Compiler

Compiler for a subset of Pascal as described in _Compilers: Principles, Techniques, and Tools 1st Edition - Aho, Sethi, Ullman, appendix A_.
The grammar has been modified to support nested subprogram definitions.

## Requirements
* a linux environment
* clang
* GNU Make

## Build
```bash
make clean
make
```

## Run
By default, the compiler will read  and analyze the `data/program.pas` source file:
```bash
./bin/pasc
```

A different source file may be specified on the command line:
```bash
./bin/pasc /path/to/source.pas
```

After execution, a `listingfile`, `symboltable`, and `tokenfile` will be output to the project root directory.
