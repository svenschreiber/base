#ifndef PLATFORM_H
#define PLATFORM_H

// =============================
// >> GENERAL

#define PLATFORM_DEFAULT_WINDOW_WIDTH  1280
#define PLATFORM_DEFAULT_WINDOW_HEIGHT 720


// =============================
// >> PLATFORM STRUCTS

typedef struct Platform_File Platform_File;
struct Platform_File {
    u64 size;
    u8 *data;
};

typedef struct Platform_State Platform_State;
struct Platform_State {
    s32 window_width;
    s32 window_height;
    b32 running;
};

static Platform_State *platform_state = 0;


// =============================
// >> PLATFORM PROCEDURES

void platform_log(char *format, ...);
b32 platform_read_entire_file(char *file_name, Platform_File *result);
void* platform_reserve_memory(u64 size);
void platform_commit_memory(void *mem, u64 size);
void platform_release_memory(void *mem);
void platform_decommit(void *mem, u64 size);

#endif
