/* string.h - v0.1 - Sven A. Schreiber
 *
 * string.h is a single header file library for string allocation
 * and modification. It is part of and depends on my C base-layer.
 *
 * To use this file simply define STRING_IMPL once at the start of
 * your project before including it. After that you can include it 
 * without defining STRING_IMPL as per usual.
 * 
 * Example:
 * ...
 * #define STRING_IMPL
 * #include "string.h"
 * ...
 */

#ifndef STRING_H
#define STRING_H

// For va_start, etc.
#include <stdarg.h> 

// +============+
// | DEFINTIONS |
// +============+

typedef struct String String;
struct String {
    char *str;
    u64 size;
};


// +===========+
// | INTERFACE |
// +===========+

String str_push(Mem_Arena *arena, char *str);
String str_pushf(Mem_Arena *arena, char *format, ...);
String str_copy(Mem_Arena *arena, String string);
String str_concat(Mem_Arena *arena, String a, String b);

// +================+
// | IMPLEMENTATION |
// +================+

#ifdef STRING_IMPL

String str_push(Mem_Arena *arena, char *str) {
    u64 len = strlen(str);
    String result = {0};
    result.str = PushData(arena, char, len);
    result.size = len;
    memmove(result.str, str, len);
    return result;
}

String str_pushf(Mem_Arena *arena, char *format, ...) {
    char buffer[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 2048, format, args);
    String result = str_push(arena, &buffer[0]);
    va_end(args);
    return result;
}

String str_copy(Mem_Arena *arena, String str) {
    String result = {0};
    result.str = PushData(arena, char, str.size);
    result.size = str.size;
    memmove(result.str, str.str, str.size);
    return result;
}

String str_concat(Mem_Arena *arena, String a, String b) {
    String result = {0};
    u64 size = a.size + b.size;
    result.str = PushData(arena, char, size);
    result.size = size;
    memmove(result.str, a.str, a.size);
    memmove(result.str + a.size, b.str, b.size);
    return result;
}

#endif

#endif
