#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// +============+
// | DEFINTIONS |
// +============+

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef s32 b32;

typedef float f32;
typedef double f64;


// +===============+
// | HELPER MACROS |
// +===============+

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

// These are actually KiB, MiB,... but everyone knows that it is a power of two.
#define KB(n) ((n) << 10)
#define MB(n) ((n) << 20)
#define GB(n) ((u64)(n) << 30)

#define Assert(expression) if(!(expression)) { *(int *)0 = 0; }



/////////////////////////////
// Stack
#define Custom_Stack_Push(s, n, first, next) ((s)->first ? ((n)->next = (s)->first, \
                                                            (s)->first = (n)) : \
                                              ((s)->first = (n)))
#define Stack_Push(s, n) Custom_Stack_Push(s, n, first, next)
#define Custom_Stack_Pop(s, first, next) ((s)->first ? (s)->first = (s)->first->next : (s)->first)
#define Stack_Pop(s) Custom_Stack_Pop(s, first, next)



/////////////////////////////
// Queue
#define Custom_Queue_PushBack(q, n, first, last, next) (((q)->last ? (q)->last->next = (n) : \
                               ((q)->last = (n), (q)->first = (n))),\
                              (q)->last = (n))

#define Queue_PushBack(q, n) Custom_Queue_PushBack(q, n, first, last, next)
#define Custom_Queue_PopFront(q, first, next) Custom_Stack_Pop(q, first, next)
#define Queue_PopFront(q) Custom_Queue_PopFront(q, first, next)


/////////////////////////////
// Doubly-linked-list
#define Custom_DLL_PushBack(dll, n, first, last, next, prev) (((dll)->last ? \
                               ((n)->prev = (dll)->last, (dll)->last->next = (n)) : \
                               ((dll)->first = (n))), ((dll)->last = (n)))

#define DLL_PushBack(dll, n) Custom_DLL_PushBack(dll, n, first, last, next, prev)
#define Custom_DLL_Remove(dll, n, first, last, next, prev) (((dll)->first == (n) ? \
                                                             ((dll)->first = (n)->next, \
                                                              ((dll)->first != 0 ? \
                                                               (dll)->first->prev = 0 : \
                                                               (dll)->first)) : \
                                                             ((n)->prev->next = (n)->next)), \
                                                            ((dll)->last == (n) ? \
                                                             ((dll)->last = (n)->prev, \
                                                              ((dll)->last != 0 ? \
                                                               (dll)->last->next = 0 : \
                                                               (dll)->last)) : \
                                                             ((n)->next->prev = (n)->prev)))
#define DLL_Remove(dll, n) Custom_DLL_Remove(dll, n, first, last, next, prev)
#endif
