 #ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// winslop shit
#ifdef _WIN32
#define STR_API __declspec(dllexport)
#else // unix:
#define STR_API __attribute__((visibility("default")))
#endif

typedef struct {
  char* data;
  size_t len; // does not include \0
  size_t cap;
} string;

typedef struct {
  const char* data;
  size_t len; // does not include \0
} string_view;

// assumes xs as NON-NULL pointer (you have to check)
#define da_append(xs, x) do {                                           \
    size_t new_cap = (xs)->cap;                                         \
    if ((xs)->len >= new_cap) {                                         \
      new_cap = new_cap == 0 ? 64 : new_cap * 2;                        \
      (xs)->data = realloc((xs)->data, new_cap * sizeof(*(xs)->data));  \
    }                                                                   \
    if ((xs)->data) {                                                   \
      (xs)->data[(xs)->len++] = x;                                      \
      (xs)->cap = new_cap;                                              \
    }                                                                   \
  } while (0)

/*
  Makes and returns string struct from C-strings with length
  If you have len value, use this
  But if you dont have and if you're using strlen, you can use
  str_from_cstr, it does strlen
 */
STR_API string str_newn(const char* buf, size_t len);

/*
  str_newn with no len value, it calculates len via strlen
  If you dont have your string's length value or just dont want
  to use strlen by yourself, you can use this
 */
STR_API string str_new(const char* buf);

/*
  Constructs a new string from string_view
*/
STR_API string str_new_from_sv(const string_view* sv);

/*
  Makes new string_view from given string
*/
STR_API string_view sv_from_str(const string* s);

/*
  Makes new string view from given string with an offset value
  if offset is bigger than string length, it returns empty string_view
  s.data=hello\0
  s.len=5
  offset=0 => sv.data = hello\0, sv.len=5
  offset=1 => sv.data = ello\0, sv.len=4
  offset=4 => sv.data = o\0, sv.len=1
  offset=5 => sv.data = \0, sv.len=0
  offset>5 => sv.data = \0, sv.len=0
*/
STR_API string_view sv_from_str_o(const string* s, size_t offset);

/*
  Reservers needed memory for the string
  It usually multiplies it by 2 if string is too long
  Extra is additional bytes to append
 */
STR_API bool str_reserve(string* s, size_t extra);

/*
  Returns char in that string by a pos value
  pos stands for position (the index, starts from zero)
 */
STR_API char str_idx(const string* s, size_t pos);

/*
  It appends char into that string
  Automatically puts \0 at the end
 */
STR_API bool str_append(string* s, char c);

/*
  Str_cat but with C-type strings (char*)
  "bufsz" is length of buf.
*/
STR_API bool str_catn(string* s, const char* buf, size_t bufsz);

/*
  Wrapper of str_append_manyn, calls strlen on buf
 */
STR_API bool str_cat(string* s, const char* buf);

/*
  It concatenates dest string and src string
  It puts src string's chars into dest string and
  puts \0 at the end and updates count and capacity (if needed)
 */
STR_API bool str_cat_str(string* dest, const string* src);

/*
  It gets "data" field in that string
  You can always use s->data or s.data
  But this checks NULL or empty conditions
 */
STR_API char* str_to_cstr(const string* s);

/*
  Calls "free" in stdlib.h and clears the string
  Use this by your hand if you dont want memory leaks
 */
STR_API void str_free(string* s);

/*
  It clears the string, sets count to zero
  and makes data's first char \0, NOT NULL!
  Technically string is clear but it does have trash
  Use this function, this is way faster than zeroing all the data
  Capacity still there and this function DOES NOT FREE THE MEMORY!
 */
STR_API void str_clear(string* s);

/*
  Nukes whitespaces at the end and begin
 */
STR_API void str_trim(string* s);

/*
  Closes the string if not closed or corrupted
  Inserts \0 at the end (depends on count field)
 */
STR_API void str_close(string* s);

/*
  Checks if string is ended with \0
 */
STR_API bool str_is_closed(const string* s);

/*
  Repeats string with provided count, modifies original string
  It appends that string at the end count - 1 times
  that means if you provide "xx" as string and 4 as count,
  you get: "xxxxxxxx" (=4*"xx")
 */
STR_API void str_repeat(string* s, size_t count);

/*
  Formats the string with given format string and variadics
  Acts like running a temporary snprintf and applying it to string
*/
STR_API void str_format_into(string* s, const char* fmt, ...);

/*
  Makes all alpha characters in that string lowercased - ASCII only
 */
STR_API void str_tolower(string* s);

/*
  Makes all alpha characters in that string uppercased - ASCII only
*/
STR_API void str_toupper(string* s);

/*
  Checks if all of chars in that string is alpha - ASCII only
 */
STR_API bool str_isalpha(const string* s);

/*
  Checks if all chars in string is alphanumeric - ASCII only
 */
STR_API bool str_isalphanum(const string* s);

/*
  Capitalizes string like this:
  hello -> Hello
  heLLo -> Hello
  hell1o -> Hello1o
  Doesn't touch non-alpha chars
  Uppers first char if it is alpha and lowers the rest (alpha ones)
  1hello -> 1hello (because 1 is not alpha)
 */
STR_API void str_capitalize(string* s);

string str_newn(const char* buf, size_t len) {
  string s = {0};
  if (!buf) return s;

  size_t cap = len + 16; // this 16 is for pre-allocation
  if (!str_reserve(&s, cap)) return s;

  memcpy(s.data, buf, len);
  s.data[len] = '\0';
  s.len = len;
  return s;
}

string str_new(const char* buf) {
  return str_newn(buf, strlen(buf));
}

string str_new_from_sv(const string_view* sv) {
  string s = {0};
  if (!sv) return s;

  s = str_newn(sv->data, sv->len);
  return s;
}

string_view sv_from_str(const string* s) {
  string_view sv = {0};
  if (!s) return sv;

  sv.data = s->data;
  sv.len = s->len;

  return sv;
}

string_view sv_from_str_o(const string* s, size_t offset) {
  string_view sv = {0};
  if (!s) return sv;

  if (offset > s->len) offset = s->len;

  sv.data = s->data + offset;
  sv.len = s->len - offset;
  return sv;
}

void str_free(string* s) {
  if (!s || !s->data) return;
  free(s->data);
  s->data = NULL;
  s->len = 0;
  s->cap = 0;
}

void str_clear(string *s) {
  s->len = 0;
  if (s && s->data && s->data[0] != '\0') {
    s->data[0] = '\0';
  }
}

char str_idx(const string* s, size_t pos) {
  if (!s || !s->data) return '\0';
  if (pos >= s->len) return '\0';
  return s->data[pos];
}

bool str_append(string* s, char c) {
  if (!s) return false;
  da_append(s, c);
  return true;
}

bool str_reserve(string* s, size_t extra) {
  if (!s) return false;
  
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

bool str_catn(string* s, const char* buf, size_t bufsz) {
  if (!s || !buf || !s->data) return false;
  if (bufsz == 0) return true;
  
  if (!str_reserve(s, bufsz)) return false;

  memcpy(s->data + s->len, buf, bufsz);
  s->len += bufsz;
  s->data[s->len] = '\0';
  return true;
}

bool str_cat(string* s, const char* buf) {
  return str_catn(s, buf, strlen(buf));
}

bool str_cat_str(string* s, const string* c) {
  return str_catn(s, c->data, c->len);
}

void str_trim(string* s) {
  if (!s || !s->data || s->len == 0) return;

  // nuke the whitespaces at the end
  while (s->len > 0 && isspace((unsigned char)s->data[s->len - 1])) {
    s->len -= 1;
  }
  s->data[s->len] = '\0';
  
  // count how many spaces at the begin
  size_t start = 0;
  while (start < s->len && isspace((unsigned char)s->data[start])) {
    start += 1;
  }

  // move string by "start" characters
  if (start > 0) {
    memmove(s->data, s->data + start, s->len - start + 1);
    s->len -= start;
  }
}

void str_repeat(string* s, size_t multiplier) {
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

void str_tolower(string* s) {
  if (!s || !s->data || s->len == 0) return;
  for (size_t i = 0; i < s->len; ++i) {
    unsigned char c = (unsigned char)s->data[i];
    unsigned char is_upper = (c >= 'A') & (c <= 'Z');
    s->data[i] += is_upper * 32;
  }
}

void str_toupper(string* s) {
  if (!s || !s->data) return;
  for (size_t i = 0; i < s->len; ++i) {
    unsigned char c = (unsigned char)s->data[i];
    unsigned char is_lower = (c >= 'a') & (c <= 'z');
    s->data[i] -= is_lower * 32;
  }
}

void str_capitalize(string* s) {
  if (!s || !s->data) return;
  for (size_t i = 0; i < s->len; ++i) {
    unsigned char c = (unsigned char)s->data[i];
    if (i == 0) {
      unsigned char is_lower = (c >= 'a') & (c <= 'z');
      s->data[i] -= is_lower * 32;
    } else {
      unsigned char is_upper = (c >= 'A') & (c <= 'Z');
      s->data[i] += is_upper * 32;
    }
  }
}

void str_format_into(string* s, const char* fmt, ...) {
  if (!s) return;
  if (!s->data || s->cap == 0) {
    s->len = 0;
    return;
  }
  
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

bool str_isalpha(const string* s) {
  if (!s || !s->data) return false;
  for (size_t i = 0; i < s->len; ++i) {
    unsigned char c = (unsigned char) s->data[i];
    unsigned char is_lower = (c >= 'a') & (c <= 'z');
    unsigned char is_upper = (c >= 'A') & (c <= 'Z');
    // is not alpha mask
    unsigned char mask = !(is_lower | is_upper);
    if (mask) return false;
  }
  return true;
}

bool str_isalphanum(const string* s) {
  if (!s || !s->data) return false;
  for (size_t i = 0; i < s->len; ++i) {
    unsigned char c = (unsigned char) s->data[i];
    unsigned char is_lower = (c >= 'a') & (c <= 'z');
    unsigned char is_upper = (c >= 'A') & (c <= 'Z');
    unsigned char is_num = (c >= '0') & (c <= '9');
    // is not alphanumeric mask
    unsigned char mask = !(is_lower | is_upper | is_num);
    if (mask) return false;
  }
  return true;
}

void str_close(string* s) {
  if (!s || !s->data) return;
  s->data[s->len] = '\0';
}

bool str_is_closed(const string* s) {
	return s->data[s->len] == '\0' ? true : false;
}

char* str_to_cstr(const string* s) {
  return s->data;
}

#endif // STR_H
