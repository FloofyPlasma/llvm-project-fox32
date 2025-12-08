# LLVM fox32
This is a fork of LLVM 22 (development version) with a backend for the [fox32](https://github.com/fox32-arch/fox32) architecture. The goal is translating LLVM IR to fox32 assembly using `llc`.

For general LLVM information, see [README-LLVM.md](README-LLVM.md).

## Building

Recommended build invocation:

```
cmake -S llvm -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_BUILD_TESTS=ON \
  -DLLVM_PARALLEL_LINK_JOBS=8 \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache
```

`ccache` reduces rebuild time. Useful due to frequent iteration.

Then build:

```
cmake --build build --target llc
```

## Usage

> [!NOTE]
> The fox32 backend is set as the default triple. You do not need to explicty set it.

Feed LLVM IR to `llc`:

```
build/bin/llc input.ll -o output.s
```

The output is fox32 assembly. As of now you will have to use the [fox32 assembler](https://github.com/fox32-arch/fox32asm) to assemble the output.


## License

This backend, and the LLVM Project are licensed under the Apache License v2.0 with LLVM Exceptions. See the [LICENSE](LICENSE.TXT) for more information.
