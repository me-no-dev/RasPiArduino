#ifndef Character_h
#define Character_h

#include <ctype.h>

inline boolean isAlphaNumeric(int c) __attribute__((always_inline));
inline boolean isAlpha(int c) __attribute__((always_inline));
inline boolean isAscii(int c) __attribute__((always_inline));
inline boolean isWhitespace(int c) __attribute__((always_inline));
inline boolean isControl(int c) __attribute__((always_inline));
inline boolean isDigit(int c) __attribute__((always_inline));
inline boolean isGraph(int c) __attribute__((always_inline));
inline boolean isLowerCase(int c) __attribute__((always_inline));
inline boolean isPrintable(int c) __attribute__((always_inline));
inline boolean isPunct(int c) __attribute__((always_inline));
inline boolean isSpace(int c) __attribute__((always_inline));
inline boolean isUpperCase(int c) __attribute__((always_inline));
inline boolean isHexadecimalDigit(int c) __attribute__((always_inline));
inline int toAscii(int c) __attribute__((always_inline));
inline int toLowerCase(int c) __attribute__((always_inline));
inline int toUpperCase(int c)__attribute__((always_inline));

inline boolean isAlphaNumeric(int c) {
  return ( isalnum(c) == 0 ? false : true);
}

inline boolean isAlpha(int c){
  return ( isalpha(c) == 0 ? false : true);
}

inline boolean isAscii(int c){
  return ( isascii (c) == 0 ? false : true);
}

inline boolean isWhitespace(int c){
  return ( isblank (c) == 0 ? false : true);
}

inline boolean isControl(int c){
  return ( iscntrl (c) == 0 ? false : true);
}

inline boolean isDigit(int c){
  return ( isdigit (c) == 0 ? false : true);
}

inline boolean isGraph(int c){
  return ( isgraph (c) == 0 ? false : true);
}

inline boolean isLowerCase(int c){
  return (islower (c) == 0 ? false : true);
}

inline boolean isPrintable(int c){
  return ( isprint (c) == 0 ? false : true);
}

inline boolean isPunct(int c){
  return ( ispunct (c) == 0 ? false : true);
}

inline boolean isSpace(int c){
  return ( isspace (c) == 0 ? false : true);
}

inline boolean isUpperCase(int c){
  return ( isupper (c) == 0 ? false : true);
}

inline boolean isHexadecimalDigit(int c){
  return ( isxdigit (c) == 0 ? false : true);
}

inline int toAscii(int c){
  return toascii (c);
}

inline int toLowerCase(int c){
  return tolower (c);
}

inline int toUpperCase(int c){
  return toupper (c);
}

#endif