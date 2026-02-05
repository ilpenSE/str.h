# str.h Library - std::string's in C

This header gives you similar implementation of std::string's. You can use this in your project or anywhere else.

## Implementation

This is STB-Style single header library. You download the .h file and include it but define **STR_IMPLEMENTATION** at somewhere once.

Simple example:
```c
#define STR_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main() {
  string s = str_new("Hello, World!");
  
  printf("%s\n", s.data); // Hello, World!
  printf("%c\n", str_idx(&s, 0)); // H
  
  str_clear(&s); // clears the string
  // current memory: "\0Hello, World!\0"
  //                  ^^ -> s.data points there
  printf("\"%s\"\n", s.data); // "" (empty string)
  printf("%zu\n", s.count); // 0
  
  str_free(&s); // frees and makes NULL the string
  printf("%p\n", s); // (null) / (nil)
  printf("%s\n", s.data); // UB/Segfault
}
```

## String Manipulations

We have `tests/appends.c`, `tests/trims.c` etc. in tests folder you can test the features

- Use `str_cat` instead of `da_append_many` because it is too slow.
- In isalpha, isalphanumeric and tolower/toupper functions, I used functions from ctype.h because the UTF-8 encoding
- For now, we dont have any optimizations like small-string optimization (SSO) or single-instruction multiple-data (SIMD)

## About

2026, ilpeN or ilpenSE

[YouTube](https://youtube.com/@ilpenwastaken)
[Instagram](https://instagram.com/ilpenwastaken)
[X](https://x.com/@ilpenwastaken)
