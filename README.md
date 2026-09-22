# Modern C String Implementation - str.h

![GitHub](https://img.shields.io/badge/ilpenSE-str.h-blue?logo=github)

![C Single Header](https://img.shields.io/badge/C-Single%20Header-005697)
![Public Domain](https://img.shields.io/badge/Public%20Domain-green)

- This stb-style single-header provides both dynamic/growable strings or static/immutable string views in UTF-8.
- License: MIT
- It requires minimum C11 version of C for safety. It compiles in C99 but some realloc and free behaviors aren't standartized and the code depends on it.
- C++ support: `assigning to 'char *' from incompatible type 'void *'`, `cannot initialize a variable of type 'char *' with an rvalue of type 'const void *'`, `invalid suffix on literal; C++11 requires a space between literal and identifier` (this is what C++ standarts say)

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
