#define STR_IMPLEMENTATION
#include "str.h"
#include <stdio.h>

int main(void) {
  printf("------------------------------\n");
  {
    String s = STR("merhaba");
    if (STR_INVALID(s)) {
      fprintf(stderr, "ERROR: String 's' failed to initialize\n");
      return 1;
    }

    printf("s = "STR_FMT"\n", STR_ARG(s));
    str_concat(&s, SVL(" kanka"));
    printf("s = "STR_FMT"\n", STR_ARG(s));
    str_free(&s);
  }
  printf("------------------------------\n");

  printf("------------------------------\n");
  {
    StringView sv = SVL("merhaba kanka dünya millet");
    printf("sv = "SV_FMT"\n", SV_ARG(sv));

    while (sv.count > 0) {
      StringView chopped = sv_chop_until_delim(&sv, ' ');
      printf("chopped = "SV_FMT"\n", SV_ARG(chopped));
    }
  }
  printf("------------------------------\n");

  printf("------------------------------\n");
  {
    StringView sv1 = SVL("  selam");
    StringView sv2 = SVL("selam  ");
    StringView sv3 = SVL("  selam  ");

    sv_trim_left(&sv1);
    sv_trim_right(&sv2);
    sv_trim(&sv3);

    printf("sv1 = "SV_FMT"\n", SV_ARG(sv1));
    printf("sv2 = "SV_FMT"\n", SV_ARG(sv2));
    printf("sv3 = "SV_FMT"\n", SV_ARG(sv3));
  }
  printf("------------------------------\n");

  printf("------------------------------\n");
  {
    StringView sv = SVL("selam\taga");
    StringView chopped = sv_chop_by_func(&sv, isspace);
    printf("sv = '"SV_FMT"'\n", SV_ARG(sv));
    printf("chopped = '"SV_FMT"'\n", SV_ARG(chopped));
  }
  printf("------------------------------\n");

}
