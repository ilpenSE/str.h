#define STR_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
  String s = str_from(SVL("merhaba"));
  if (STR_INVALID(s)) {
    fprintf(stderr, "ERROR: String 's' failed to initialize\n");
    return 1;
  }

  printf("s = "STR_FMT"\n", STR_ARG(s));
  str_concat(&s, SVL(" kanka"));
  printf("s = "STR_FMT"\n", STR_ARG(s));

  StringView sv = SVL("merhaba kanka dünya millet");
  printf("sv = "SV_FMT"\n", SV_ARG(sv));

  while (sv.count > 0) {
    StringView chopped = sv_chop_until_delim(&sv, ' ');
    printf("chopped = "SV_FMT"\n", SV_ARG(chopped));
  }

  str_free(&s);
}
