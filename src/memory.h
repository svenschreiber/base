/* memory.h - v0.1 - Sven A. Schreiber
 *
 * memory.h is a single header file library for allocation and
 * management of memory. It is part of and depends on my C base-layer.
 * 
 * To use this file simply define MEMORY_IMPL once at the start of
 * your project before including it. After that you can include it 
 * without defining MEMORY_IMPL as per usual.
 * 
 * Example:
 * ...
 * #define MEMORY_IMPL
 * #include "memory.h"
 * ...
 */

#ifndef MEMORY_H
#define MEMORY_H

// +============+
// | DEFINTIONS |
// +============+

// =========================
// >> Memory Arena

#define MEM_ARENA_MAX GB(2)
#define MEM_ARENA_COMMIT_SIZE KB(4)
#define MEM_ARENA_ALIGN_DEFAULT 8

typedef struct Mem_Arena Mem_Arena;
struct Mem_Arena {
    u64 max;
    u64 alloc_pos;
    u64 commit_pos;
    void *data;
    u64 align;
};


// =========================
// >> Memory Heap
//
// This heap implementation is heavily inspired by:
// https://github.com/CCareaga/heap_allocator
// https://www.cs.tufts.edu/~nr/cs257/archive/doug-lea/malloc.html

#define MEM_HEAP_BUCKETS_MAX 64
#define MEM_HEAP_MIN_ALLOC_SIZE 4

typedef struct Mem_Chunk Mem_Chunk;
struct Mem_Chunk {
    u64 size;
    Mem_Chunk *next;
};

typedef struct Mem_Heap_Bucket Mem_Heap_Bucket;
struct Mem_Heap_Bucket {
    Mem_Chunk *first;
};

typedef struct Mem_Heap Mem_Heap;
struct Mem_Heap {
    Mem_Arena arena;
    Mem_Heap_Bucket *buckets;
};


// +===========+
// | INTERFACE |
// +===========+

Mem_Arena mem_arena_init_with_align(u64 align, u64 size);
Mem_Arena mem_arena_init(u64 size);
void *mem_arena_push(Mem_Arena *arena, u64 size);
void *mem_arena_push_zero(Mem_Arena *arena, u64 size);
void mem_arena_pop(Mem_Arena *arena, u64 size);
void mem_arena_release(Mem_Arena *arena);
void mem_arena_clear(Mem_Arena *arena);

Mem_Heap mem_heap_init(u64 size);
void *mem_heap_alloc(Mem_Heap *heap, u64 size);
void mem_heap_free(Mem_Heap *heap, void *data);
u32 mem_heap_get_index(u64 size);
Mem_Chunk *mem_chunk_get_or_make(Mem_Arena *arena, Mem_Heap_Bucket *bucket, u64 size);
Mem_Chunk *mem_chunk_make(Mem_Heap_Bucket *bucket, u64 size);
void mem_heap_release(Mem_Heap *heap);

u64 round_up_next_pow2(u64 n);

// +===============+
// | HELPER MACROS |
// +===============+

#define PushData(arena,T,c) ( (T*)(mem_arena_push((arena),sizeof(T)*(c))) )
#define PushDataZero(arena,T,c) ( (T*)(mem_arena_push_zero((arena),sizeof(T)*(c))) )
#define PushStruct(arena,T) PushData(arena,T,1);
#define PushStructZero(arena, T) PushDataZero(arena, T, 1);


// +================+
// | IMPLEMENTATION |
// +================+

#ifdef MEMORY_IMPL

Mem_Arena mem_arena_init_with_align(u64 align, u64 size) {
    Mem_Arena arena = {0};
    arena.max        = size;
    arena.data       = platform_reserve_memory(arena.max);
    arena.alloc_pos  = 0;
    arena.commit_pos = 0;
    arena.align      = align;
    return arena;
}

Mem_Arena mem_arena_init(u64 size) {
    return mem_arena_init_with_align(MEM_ARENA_ALIGN_DEFAULT, size);
}

void *mem_arena_push(Mem_Arena *arena, u64 size) {
    void *mem = 0;
    if (arena->alloc_pos + size > arena->commit_pos) {
        u64 commit_size = size;
        commit_size += MEM_ARENA_COMMIT_SIZE - 1;
        commit_size -= commit_size % MEM_ARENA_COMMIT_SIZE;
        platform_commit_memory((u8 *)arena->data + arena->commit_pos, commit_size);
        arena->commit_pos += commit_size;
    }
    mem = (u8 *)arena->data + arena->alloc_pos;
    u64 pos = arena->alloc_pos + size;
    arena->alloc_pos = (pos + arena->align - 1) & (~(arena->align - 1));
    return mem;
}

void mem_arena_pop(Mem_Arena *arena, u64 size) {
    if (size > arena->alloc_pos) {
        size = arena->alloc_pos;
    }
    arena->alloc_pos -= size;
}

void mem_arena_release(Mem_Arena *arena) {
    platform_release_memory(arena->data, arena->max);
}

void mem_arena_clear(Mem_Arena *arena) {
    arena->alloc_pos = 0;
}

void *mem_arena_push_zero(Mem_Arena *arena, u64 size) {
    void *mem = mem_arena_push(arena, size);
    memset(mem, 0, size);
    return mem;
}

Mem_Heap mem_heap_init(u64 size) {
    Mem_Heap result = {0};
    result.arena = mem_arena_init(size);
    result.buckets = PushDataZero(&result.arena, Mem_Heap_Bucket, MEM_HEAP_BUCKETS_MAX);
    return result;
}

void *mem_heap_alloc(Mem_Heap *heap, u64 size) {
    u64 chunk_size = round_up_next_pow2(size + sizeof(Mem_Chunk));
    u32 bucket_idx = mem_heap_get_index(chunk_size);
    Mem_Heap_Bucket *bucket = &heap->buckets[bucket_idx];
    Mem_Chunk *chunk = mem_chunk_get_or_make(&heap->arena, bucket, chunk_size);
    Assert(chunk);
    return (void *)(chunk + 1);
}

void mem_heap_free(Mem_Heap *heap, void *data) {
    Mem_Chunk *chunk = (Mem_Chunk *)((u8 *)data - sizeof(Mem_Chunk));
    memset(data, 0, chunk->size - sizeof(Mem_Chunk));
    u32 bucket_idx = mem_heap_get_index(chunk->size);
    Mem_Heap_Bucket *bucket = &heap->buckets[bucket_idx];
    Stack_Push(bucket, chunk);
}

Mem_Chunk *mem_chunk_get_or_make(Mem_Arena *arena, Mem_Heap_Bucket *bucket, u64 chunk_size) {
    Mem_Chunk *result = 0;
    if (bucket->first) {
        result = bucket->first;
        Stack_Pop(bucket);
    } else {
        u8 *data = PushDataZero(arena, u8, chunk_size);
        result = (Mem_Chunk *)data;
        result->size = chunk_size;
    }
    return result;
}

u32 mem_heap_get_index(u64 chunk_size) {
    u32 index = 0;
    chunk_size =  chunk_size < MEM_HEAP_MIN_ALLOC_SIZE ? MEM_HEAP_MIN_ALLOC_SIZE : chunk_size;

    while (chunk_size >>= 1) ++index;
    index -= 2;
    return index;
}

u64 round_up_next_pow2(u64 n) {
    if (n == 0) return 1;
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    ++n;
    return n;
}

void mem_heap_release(Mem_Heap *heap) {
    mem_arena_release(&heap->arena);
}

#endif

#endif

