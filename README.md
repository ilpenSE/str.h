# Modern C String Implementation - str.h

![GitHub](https://img.shields.io/badge/ilpenSE-str.h-blue?logo=github)
[![CI](https://github.com/ilpenSE/str.h/actions/workflows/build.yaml/badge.svg)](https://github.com/ilpenSE/str.h/actions/workflows/build.yaml)

![C Single Header](https://img.shields.io/badge/C-Single%20Header-005697)
![Public Domain](https://img.shields.io/badge/Public%20Domain-green)

- This stb-style single-header provides both dynamic/growable strings or static/immutable string views in UTF-8.
- License: MIT
- It requires minimum C11 version of C for safety. It compiles in C99 but some realloc and free behaviors aren't standartized and the code depends on it.

## Quick Start

- Download `str.h` file into your include folder/path.
- Define `STR_IMPLEMENTATION` macro somewhere once that you include the header like this:
```c
#define STR_IMPLEMENTATION
#include "str.h"

int main() {
  String s = STR("Hello, World!");
  printf("s = %s\n", s.data);
  printf("s = "STR_FMT"\n", STR_ARG(s));
}
```

## Documentation

- Code is self-documented. But you can read documentation of each individual function in the comment where's top of its decleration
