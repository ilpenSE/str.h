#include <stdio.h>
#include "prettyp.h"

int main() {
  string s = str_new("Hello, World!");
  string_view sv1 = sv_from_str(&s); // Hello, World!
  string_view sv2 = sv_from_str_o(&s, 4); // o, World!
  string_view sv3 = sv_from_str_o(&s, 50); // empty
  string_view sv4 = sv_from_str_o(&s, 0); // Hello, World!
  
  print_sv(&sv1);
  print_sv(&sv2);
  print_sv(&sv3);
  print_sv(&sv4);

  str_free(&s);
  return 0;
}
