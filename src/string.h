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
    u8 *str;
    u64 size;
};

typedef struct String_List_Node String_List_Node;
struct String_List_Node {
    String_List_Node *next;
    String string;
};

typedef struct String_List String_List;
struct String_List {
    String_List_Node *first;
    String_List_Node *last;
    u64 num_nodes;
};


// +===========+
// | INTERFACE |
// +===========+

String str_push(Mem_Arena *arena, char *str);
String str_pushf(Mem_Arena *arena, char *format, ...);
String str_copy(Mem_Arena *arena, String string);
String str_substring(String str, u64 from, u64 to);
char *str_null_termintate(Mem_Arena *arena, String str);
String str_concat(Mem_Arena *arena, String a, String b);
String_List str_split(Mem_Arena *arena, String str, char c);
b32 str_equal(String a, String b);

void str_list_push_node(String_List *list, String_List_Node *node);
void str_list_push(Mem_Arena *arena, String_List *list, String str);
// TODO:
// str_list_concat()
// str_list_join()


// +================+
// | IMPLEMENTATION |
// +================+

#ifdef STRING_IMPL

String str_push(Mem_Arena *arena, char *str) {
    u64 len = strlen(str);
    String result = {0};
    result.str = PushData(arena, u8, len);
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
    result.str = PushData(arena, u8, str.size);
    result.size = str.size;
    memmove(result.str, str.str, str.size);
    return result;
}

String str_concat(Mem_Arena *arena, String a, String b) {
    String result = {0};
    u64 size = a.size + b.size;
    result.str = PushData(arena, u8, size);
    result.size = size;
    memmove(result.str, a.str, a.size);
    memmove(result.str + a.size, b.str, b.size);
    return result;
}

String str_substring(String str, u64 from, u64 to) {
    String result = {0};
    if (from > str.size) from = str.size;
    if (to > str.size) to = str.size;
    if (from > to) {
        u64 tmp = from;
        from = to;
        to = tmp;
    }
    str.str += from;
    str.size = to - from;
    return result;
}

char *str_null_terminate(Mem_Arena *arena, String str) {
    if ((str.size > 0) && (str.str[str.size - 1] != '\0')) {
        return (char *)str_concat(arena, str, str_push(arena, "\0")).str;
    }
    return (char *)str.str;
}

String_List str_split(Mem_Arena *arena, String str, char c) {
    String_List result = {0};
    u64 start = 0;
    for (u64 i = 0; i < str.size; ++i) {
        if (str.str[i] == c) {
            str_list_push(arena, &result, str_substring(str, start, i));
            start = i + 1;
        }
    }
    str_list_push(arena, &result, str_substring(str, start, str.size));
    return result;
}

b32 str_equal(String a, String b) {
    if (a.size != b.size) return 0;
    for (u64 i = 0; i < a.size; ++i) {
        if (a.str[i] != b.str[i]) {
            return 0;
        }
    }
    return 1;
}

void str_list_push_node(String_List *list, String_List_Node *node) {
    Queue_PushBack(list, node);
    list->num_nodes += 1;
}

void str_list_push(Mem_Arena *arena, String_List *list, String str) {
    String_List_Node *node = PushStructZero(arena, String_List_Node);
    node->string = str;
    str_list_push_node(list, node);
}

#endif

#endif
