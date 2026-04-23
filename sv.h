#ifndef SV_H
#define SV_H

#ifdef __cplusplus
#define SVDEF extern "C"
#else
#define SVDEF extern
#endif

#include <stddef.h>

#ifndef __StringView_defined
#define __StringView_defined
typedef struct {
  const char* data; // not null terminated
  size_t len;
} StringView;
#endif // __StringView_defined

#ifndef __String_defined
#define __String_defined
typedef struct {
  char* data;
  size_t len; // does not include \0
  size_t cap;
} String;
#endif // __String_defined

#ifndef __uchar_t_defined
typedef unsigned char uchar_t;
#define __uchar_t_defined
#endif

#define SV_ARG(sv) (int)(sv)->len, (sv)->data
#define SV_FMT "%.*s"

// Bridge functions between str.h and sv.h
// String must be defined, including str.h defines it
#ifdef __String_defined
/*
  Slices heap-allocated string, wrapper to sv_from_cstr
*/
SVDEF StringView sv_from_str(const String* s, size_t start, size_t end);
#endif // __String_defined

/*
  Slices string in range [start, end] and makes new StringView
  Length, start and end are checked
*/
SVDEF StringView sv_from_cstre(const char* buf, size_t len, size_t start, size_t end);

/*
  Produce StringView from zero-ended const strings without explicit start and end
*/
SVDEF StringView sv_from_cstr(const char* buf);

/*
  Take sub string from string view and return the sub string
*/
SVDEF StringView sv_substr(StringView sv, size_t start, size_t len);

/*
  Trims the string view from left or right or both sides
*/
SVDEF StringView sv_trim_left(StringView sv);
SVDEF StringView sv_trim_right(StringView sv);
SVDEF StringView sv_trim(StringView sv);

/*
  Chop n characters from left or right
  modifies given sv and constructs removed part then returns it
*/
SVDEF StringView sv_chop_left(StringView* sv, size_t n);
SVDEF StringView sv_chop_right(StringView* sv, size_t n);

// Synonyms for chops
#define sv_remove_prefix sv_chop_left
#define sv_remove_suffix sv_chop_right

/*
  Writes sv's data with length into zero-terminated string
  DICLAIMER: make sure that out buffer's size is enough
  otherwise you will get buffer overflow
*/
SVDEF void sv_tostr(const StringView* sv, char* out);

/*
  Compares 2 string views and check if they're equals
  Basically: sv_cmp(lhs, rhs) == 0 like strcmp(lhs, rhs) == 0
*/
SVDEF bool sv_equals(const StringView* lhs, const StringView* rhs);

/*
  Compares 2 string views
  returns zero     if lhs == rhs
  returns positive if lhs >  rhs
  returns negative if lhs <  rhs
  First, diff lengths: lhs.len - rhs.len (will be pos if lhs's length is greater)
  Then check buffers byte by byte and if any different byte found in same length,
  return it. Simply: memcmp(lhs, rhs, lhs.len)
*/
SVDEF int sv_cmp(const StringView* lhs, const StringView* rhs);

/*
  Clears the string view, making length zero
  and pointer to null
*/
SVDEF void sv_clear(StringView *sv);

/*
  Check if data pointer points to zero and length is also zero
*/
SVDEF bool sv_isempty(const StringView* sv);

#ifdef SV_IMPLEMENTATION

#include <string.h>
#include <ctype.h>

StringView sv_from_cstre(const char* buf, size_t len, size_t start, size_t end) {
  StringView sv = {0};
  if (start > end || start > len || end > len) return sv;
  sv.data = buf + start;
  sv.len = end - start;
  return sv;
}

StringView sv_from_cstr(const char* buf) {
  size_t buflen = strlen(buf);
  return sv_from_cstre(buf, buflen, 0, buflen);
}

#ifdef __String_defined
StringView sv_from_str(const String* s, size_t start, size_t end) {
  return sv_from_cstre(s->data, s->len, start, end);
}
#endif // __String_defined

void sv_tostr(const StringView* sv, char* out) {
  if (!out || !sv) return;
  memcpy(out, sv->data, sv->len);
  out[sv->len] = '\0';
}

void sv_clear(StringView *sv) {
  if (sv == NULL) return;
  sv->len = 0;
  sv->data = "";
}

bool sv_isempty(const StringView* sv) {
  return sv->len == 0 && (!sv->data ? true : *sv->data == 0);
}

int sv_cmp(const StringView* lhs, const StringView* rhs) {
  if (lhs == rhs) return 0;
  int sdiff = (int)lhs->len - (int)rhs->len;
  if (sdiff != 0) return sdiff; // size diff
  return memcmp(lhs->data, rhs->data, lhs->len);
}

bool sv_equals(const StringView* lhs, const StringView* rhs) {
  return sv_cmp(lhs, rhs) == 0;
}

StringView sv_chop_left(StringView* sv, size_t n) {
  if (n >= sv->len) return (StringView) {0};
  StringView res = {.data=sv->data, .len=n};
  sv->data += n;
  sv->len -= n;
  return res;
}

StringView sv_chop_right(StringView* sv, size_t n) {
  StringView res = {.data=sv->data + sv->len - n, .len=n};
  sv->len -= n;
  return res;
}

StringView sv_trim_left(StringView sv) {
  while (sv.len > 0 && isspace((uchar_t)sv.data[0])) {
    sv.data++;
    sv.len--;
  }
  return sv;
}

StringView sv_trim_right(StringView sv) {
  while (sv.len > 0 && isspace((uchar_t)sv.data[sv.len - 1])){
    sv.len--;
  }
  return sv;
}

StringView sv_trim(StringView sv) {
  return sv_trim_right(sv_trim_left(sv));
}

#endif // SV_IMPLEMENTATION

#endif // SV_H
