#include <stdio.h>
#include "prettyp.h"

int main() {
  string s = str_new("selam");
  print_str(&s);
  
  str_clear(&s);
  print_str(&s);
  
  str_free(&s);
  print_str(&s);
  return 0;
}
