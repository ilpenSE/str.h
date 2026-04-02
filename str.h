#ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "vector.h"

#ifdef __cplusplus
#define STRDEF extern "C"
#else
#define STRDEF extern
#endif

typedef struct {
  char* data;
  size_t len; // does not include \0
  size_t cap;
} String;

typedef struct {
  const char* data;
  size_t len; // does not include \0
} StringView;

DECL_VECTOR(StringView, StringView);

#ifndef uchar_t
typedef unsigned char uchar_t;
#endif

#define sv_arg(sv) (int)(sv)->len, (sv)->data
#define sv_fmt "%.*s"

#define str_foreach(s, name) for(char* name = (s)->data; name < (s)->data + (s)->len; name++)

/*
  Functions that take String or StringView context
  as first argument and pointer DO NOT CHECK NULL CONTEXT!
  Like: bool str_reserve(NULL, 10); RESULTS WITH UNDEFINED BEHAVIOR!
  It's your responsibility to check them
*/

/*
  Makes and returns string struct from C-strings with length
  If you have len value, use this
  But if you dont have and if you're using strlen, you can use
  str_from_cstr, it does strlen
 */
STRDEF String str_newn(const char* buf, size_t len);

/*
  str_newn with no len value, it calculates len via strlen
  If you dont have your String's length value or just dont want
  to use strlen by yourself, you can use this
 */
STRDEF String str_new(const char* buf);

/*
  Constructs a new String from StringView
*/
STRDEF String str_new_from_sv(const StringView* sv);

/*
  Slices string in range [start, end) and makes new StringView
  Length, start and end are checked
*/
STRDEF StringView sv_from_cstre(const char* buf, size_t len, size_t start, size_t end);

/*
  Produce StringView from zero-ended const strings without explicit start and end
*/
STRDEF StringView sv_from_cstr(const char* buf);

/*
  Slices heap-allocated string, wrapper to sv_from_cstr
*/
STRDEF StringView sv_from_str(const String* s, size_t start, size_t end);

/*
  Writes String view data with length to out buffer
  Since we cannot mutate String view or we cannot return
  local stack variables in functions, we use call-stack method
  NOTE: out buffer's length check is in your responsibility.
*/
STRDEF void sv_tostr(const StringView* sv, char* out);

/*
  Reservers needed memory for the String
  It usually multiplies it by 2 if String is too long
  Extra is additional bytes to append
 */
STRDEF bool str_reserve(String* s, size_t extra);

/*
  Shrinks the memory (cap field) to length + 1
*/
STRDEF bool str_shrink_to_fit(String* s);

/*
  Returns char in that String by a pos value
  pos stands for position (the index, starts from zero)
 */
STRDEF char str_idx(const String* s, size_t pos);

/*
  It appends char into that String
  Automatically puts \0 at the end
 */
STRDEF bool str_append(String* s, char c);

/*
  Str_cat but with C-type Strings (char*)
  "bufsz" is length of buf.
*/
STRDEF bool str_catn(String* s, const char* buf, size_t bufsz);

/*
  Wrapper of str_append_manyn, calls strlen on buf
 */
STRDEF bool str_cat(String* s, const char* buf);

/*
  It concatenates dest String and src String
  It puts src String's chars into dest String and
  puts \0 at the end and updates count and capacity (if needed)
*/
STRDEF bool str_cat_str(String* dest, const String* src);

/*
  It gets "data" field in that String
  You can always use s->data or s.data
  But this checks NULL or empty conditions
*/
STRDEF char* str_to_cstr(const String* s);

/*
  Wrapper for cstr_equals
*/
STRDEF bool str_equals(const String* s, const String* other);

/*
  Wrapper for cstr_equals
*/
STRDEF bool sv_equals(const StringView* s, const StringView* other);

/*
  Check if 2 string are equal
  Make sure that buf1 and buf2's length same and n
  Checks strings char-by-char
  Not really char-by-char, first goes with 8 bytes of chunks
  at the same time then goes char-by-char
*/
STRDEF bool cstr_equals(const char* buf1, const char* buf2, size_t n);

/*
  Free the String
*/
STRDEF void str_free(String* s);

/*
  It clears the String, sets len to zero and make data \0
  Capacity still there and this function DOES NOT FREE THE MEMORY!
*/
STRDEF void str_clear(String* s);

/*
  Clears the string view, making length zero
  and pointer to null
*/
STRDEF void sv_clear(StringView *sv);

/*
  Nukes whitespaces at the end and begin
*/
STRDEF void str_trim(String* s);

/*
  Closes the String if not closed or corrupted
  Inserts \0 at the end (depends on count field)
*/
STRDEF void str_close(String* s);

/*
  Checks if String is ended with \0
*/
STRDEF bool str_is_closed(const String* s);

/*
  Repeats String with provided count, modifies original String
  It appends that String at the end count - 1 times
  that means if you provide "xx" as String and 4 as count,
  you get: "xxxxxxxx" (=4*"xx")
*/
STRDEF void str_repeat(String* s, size_t count);

/*
  Formats the String with given format String and variadics
  Acts like running a temporary snprintf and applying it to String
*/
STRDEF void str_format_into(String* s, const char* fmt, ...);

/*
  Makes all alpha characters in that String lowercased - ASCII only
 */
STRDEF void str_tolower(String* s);

/*
  Makes all alpha characters in that String uppercased - ASCII only
*/
STRDEF void str_toupper(String* s);

/*
  Checks if all of chars in that String is alpha - ASCII only
 */
STRDEF bool str_isalpha(const String* s);

/*
  Checks if all chars in String is alphanumeric - ASCII only
 */
STRDEF bool str_isalphanum(const String* s);

/*
  Capitalizes String like this:
  hello -> Hello
  heLLo -> Hello
  hell1o -> Hello1o
  Doesn't touch non-alpha chars
  Uppers first char if it is alpha and lowers the rest (alpha ones)
  1hello -> 1hello (because 1 is not alpha)
 */
STRDEF void str_capitalize(String* s);

/*
  Splits the String by char buffer delim
  Uses strtok, writes to String array from parameters
*/
STRDEF Vector(StringView) str_split(String* s, const char delim);

#ifdef STR_IMPLEMENTATION

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

typedef uintptr_t word_t;
#define WORD_SIZE sizeof(word_t)
#define WORD_MASK (WORD_SIZE - 1)

String str_newn(const char* buf, size_t len) {
  String s = {0};
  size_t cap = len + 16; // this 16 is for pre-allocation
  if (!str_reserve(&s, cap)) return s;

  memcpy(s.data, buf, len);
  s.data[len] = '\0';
  s.len = len;
  return s;
}

String str_new(const char* buf) {
  return str_newn(buf, strlen(buf));
}

String str_new_from_sv(const StringView* sv) {
  String s = {0};
  s = str_newn(sv->data, sv->len);
  return s;
}

StringView sv_from_cstre(const char* buf, size_t len, size_t start, size_t end) {
  StringView sv = {0};
  if (start > end || start > len || end > len) return sv;
  sv.data = buf + start;
  sv.len = end - start;
  return sv;
}

StringView sv_from_cstr(const char* buf) {
  size_t buflen = strlen(buf);
  return sv_from_cstre(buf, buflen, 0, buflen + 1);
}

StringView sv_from_str(const String* s, size_t start, size_t end) {
  return sv_from_cstre(s->data, s->len, start, end);
}

void sv_tostr(const StringView* sv, char* out) {
  if (!out) return;
  memcpy(out, sv->data, sv->len);
  out[sv->len] = '\0';
}

void str_free(String* s) {
  if (!s->data) return;
  free(s->data);
  s->data = NULL;
  s->len = 0;
  s->cap = 0;
}

void str_clear(String *s) {
  s->len = 0;
  if (s->data) {
    s->data[0] = '\0';
  }
}

void sv_clear(StringView *sv) {
  sv->len = 0;
  sv->data = NULL;
}

bool str_shrink_to_fit(String* s) {
  size_t new_cap = s->len + 1;
  void* tmp = realloc(s->data, new_cap);
  if (!tmp) return false;
  else s->data = (char*)tmp;
  s->cap = new_cap;
  return true;
}

char str_idx(const String* s, size_t pos) {
  if (!s->data) return '\0';
  if (pos >= s->len) return '\0';
  return s->data[pos];
}

bool str_append(String* s, char c) {
  if (!str_reserve(s, 1)) return false;

  s->data[s->len++] = c;
  s->data[s->len] = '\0';
  return true;
}

bool str_reserve(String* s, size_t extra) {
  size_t sum = extra + s->len + 1;

  // if sum is enough to fit cap, dont update cap
  if (sum <= s->cap) return true;

  // for size_t overflows
  if (sum > SIZE_MAX / 2) return false;

  // sum does NOT fit
  size_t new_cap = sum * 2;
  char *tmp = (char*)realloc(s->data, new_cap);
  if (!tmp) return false;

  s->data = tmp;
  s->cap = new_cap;
  return true;
}

bool str_catn(String* s, const char* buf, size_t bufsz) {
  if (!buf || !s->data) return false;
  if (bufsz == 0) return true;

  if (!str_reserve(s, bufsz)) return false;

  memcpy(s->data + s->len, buf, bufsz);
  s->len += bufsz;
  s->data[s->len] = '\0';
  return true;
}

bool str_cat(String* s, const char* buf) {
  return str_catn(s, buf, strlen(buf));
}

bool str_cat_str(String* s, const String* c) {
  return str_catn(s, c->data, c->len);
}

bool sv_equals(const StringView* sv, const StringView* other) {
  if (sv == other) return true;
  if (sv->len != other->len) return false;
  return cstr_equals(sv->data, other->data, sv->len);
}

bool str_equals(const String* s, const String* other) {
  if (s == other) return true;
  if (s->len != other->len) return false;
  return cstr_equals(s->data, other->data, s->len);
}

bool cstr_equals(const char* buf1, const char* buf2, size_t n) {
  const char* p = buf1;
  const char* q = buf2;

  // Alignment shit
  size_t prefix = (size_t)((-(word_t)p) & WORD_MASK);
  if (prefix > n) prefix = n;
  while (prefix--) {
    if (*p++ != *q++) return false;
    n--;
  }
  const word_t* wp = (const word_t*)p;
  const word_t* wq = (const word_t*)q;
  uint8_t diff = 0;

  // Pop it by 8 bytes of chunks
  while (n >= WORD_SIZE) {
    word_t wa, wb;
    __builtin_memcpy(&wa, wp, WORD_SIZE);
    __builtin_memcpy(&wb, wq, WORD_SIZE);
    diff |= wa ^ wb;
    wp++;
    wq++;
    n -= WORD_SIZE;
  }

  p = (const char*)wp;
  q = (const char*)wq;

  // Pop it byte by byte
  while (n--) {
    diff |= (uchar_t)(*p++) ^ (uchar_t)(*q++);
  }
  return diff == 0;
}

void str_trim(String* s) {
  if (!s->data || s->len == 0) return;

  // nuke the whitespaces at the end
  while (s->len > 0 && isspace((uchar_t)s->data[s->len - 1])) {
    s->len -= 1;
  }
  s->data[s->len] = '\0';
  
  // count how many spaces at the begin
  size_t start = 0;
  while (start < s->len && isspace((uchar_t)s->data[start])) {
    start += 1;
  }

  // move String by "start" characters
  if (start > 0) {
    memmove(s->data, s->data + start, s->len - start + 1);
    s->len -= start;
  }
}

void str_repeat(String* s, size_t multiplier) {
  if (multiplier == 1) return;
  if (multiplier == 0) return;

  size_t new_cnt = s->len * multiplier;
  if (!str_reserve(s, new_cnt - s->len)) return;

  for (size_t i = 1; i < multiplier; ++i) {
    memmove(s->data + (i * s->len), s->data, s->len);
  }
  
  s->len = new_cnt;
  s->data[new_cnt] = '\0';
}

void str_tolower(String* s) {
  if (!s->data) return;
  for (size_t i = 0; i < s->len; ++i) {
    uchar_t c = (uchar_t)s->data[i];
    uchar_t is_upper = (c >= 'A') & (c <= 'Z');
    s->data[i] += is_upper * 32;
  }
}

void str_toupper(String* s) {
  if (!s->data) return;
  for (size_t i = 0; i < s->len; ++i) {
    uchar_t c = (uchar_t)s->data[i];
    uchar_t is_lower = (c >= 'a') & (c <= 'z');
    s->data[i] -= is_lower * 32;
  }
}

void str_capitalize(String* s) {
  if (!s->data) return;
  for (size_t i = 0; i < s->len; ++i) {
    uchar_t c = (uchar_t)s->data[i];
    if (i == 0) {
      uchar_t is_lower = (c >= 'a') & (c <= 'z');
      s->data[i] -= is_lower * 32;
    } else {
      uchar_t is_upper = (c >= 'A') & (c <= 'Z');
      s->data[i] += is_upper * 32;
    }
  }
}

void str_format_into(String* s, const char* fmt, ...) {
  if (!s->data || s->cap == 0) return;

  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(s->data, s->cap, fmt, ap);
  va_end(ap);

  if (n < 0) {
    s->len = 0;
  } else if ((size_t)n >= s->cap) {
    size_t cap = s->cap;
    if (cap >= 1) {
      s->data[cap - 1] = '\0'; // guarenttes \0
      s->len = cap - 1;
    } else {
      s->len = 0;
    }
  } else {
    s->len = (size_t)n;
  }
}

bool str_isalpha(const String* s) {
  if (!s->data) return false;
  for (size_t i = 0; i < s->len; ++i) {
    uchar_t c = (uchar_t) s->data[i];
    uchar_t is_lower = (c >= 'a') & (c <= 'z');
    uchar_t is_upper = (c >= 'A') & (c <= 'Z');
    // is not alpha mask
    uchar_t mask = !(is_lower | is_upper);
    if (mask) return false;
  }
  return true;
}

bool str_isalphanum(const String* s) {
  if (!s->data) return false;
  for (size_t i = 0; i < s->len; ++i) {
    uchar_t c = (uchar_t) s->data[i];
    uchar_t is_lower = (c >= 'a') & (c <= 'z');
    uchar_t is_upper = (c >= 'A') & (c <= 'Z');
    uchar_t is_num = (c >= '0') & (c <= '9');
    // is not alphanumeric mask
    uchar_t mask = !(is_lower | is_upper | is_num);
    if (mask) return false;
  }
  return true;
}

Vector(StringView) str_split(String* s, const char delim) {
  Vector(StringView) vec = {0};

  size_t start = 0;
  size_t i = 0;
  while (i <= s->len) {
    if (i == s->len || s->data[i] == delim) {
      StringView sv = sv_from_str(s, start, i);
      vec_push(&vec, sv);
      start = i + 1;
    }
    i++;
  }
  return vec;
}

void str_close(String* s) {
  if (!s->data) return;
  s->data[s->len] = '\0';
}

bool str_is_closed(const String* s) {
  return s->data[s->len] == '\0' ? true : false;
}

char* str_to_cstr(const String* s) {
  return s->data;
}

#endif // STR_IMPLEMENTATION

#endif // STR_H
