# dke_headers
Various single-file libraries for C and C++ intended for embedded platforms,
similar to [stb libraries](https://github.com/nothings/stb).

| Library                    | Version | Description                                |
| -------------------------- | ------- | ------------------------------------------ |
| [dke_mouse.h](dke_mouse.h) | 0.1     | Transport agnostic mouse library configurable for 1-32 buttons, wheel, and pan |

These libraries are as-is, however, suggestions for improvements or bug fixes
are appreciated. Please raise an issue before submitting a PR. Bug fixes are
welcomed!

## Philosophy

These headers are built to drop right into your projects with minimal footprint
and zero architectural lock-in. Each library delivers a clean, predictable
C-style interface designed to solve its core task, leaving your application in
total control of its memory, platform I/O, and execution flow.

Use as much or as little as you need. The libraries never force a framework, an
RTOS, or rigid assumptions on your codebase. They simply provide reliable, self
contained code ready to integrate into your existing architecture.

## FAQ

### How do I use these libraries?

Generally, these headers do not have dependencies and are to be directly included
in your source code. Include them like any other header, however, you must select
exactly ONE C/C++ source file that actually instantiates the code, preferably a
file you're not editing frequently. It would look something like this:

```cpp
// Do ONCE in a c/c++ file.
#define DKE_LIBRARY_IMPLEMENTATION
#include "dke_library.h"

// Include like a typical header everywhere else.
#include "dke_library.h"
```

Check each header for specific documentation at the top of the file for what the
macro should be, if required.

### What is the license?

Each library contains license information at the end of the file. It is generally
a choice between MIT or public domain, or in the case of derived work, the
original license.

### What are the design trade-offs?

These are not high-level "plug-and-play" solutions. Where practical, each library
exposes multiple API layers so individual pieces can be used independently to
suit your specific use case.

The libraries make no assumptions about the backing platform. They will not spin
up background threads, dictate fixed memory allocations, or hide runtime state
behind your back. When concurrency is needed, the library provides discrete
functions to execute, leaving thread creation, scheduling, and synchronization
entirely to you.

In exchange for handling the integration, you get portable code with minimal or
zero dependencies, no runtime surprises, and complete control over your execution
flow.
