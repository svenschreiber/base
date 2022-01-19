#ifndef PLATFORM_H
#define PLATFORM_H

// =============================
// >> GENERAL

#define PLATFORM_DEFAULT_WINDOW_WIDTH  1280
#define PLATFORM_DEFAULT_WINDOW_HEIGHT 720


// =============================
// >> PLATFORM EVENTS

#define PLATFORM_EVENT_NONE            0x00
#define PLATFORM_EVENT_KEY_PRESS       0x01
#define PLATFORM_EVENT_KEY_RELEASE     0x02
#define PLATFORM_EVENT_CHARACTER_INPUT 0x03
#define PLATFORM_EVENT_MOUSE_PRESS     0x04
#define PLATFORM_EVENT_MOUSE_RELEASE   0x05
#define PLATFORM_EVENT_MOUSE_MOVE      0x06
#define PLATFORM_EVENT_MOUSE_SCROLL          0x07
#define PLATFORM_EVENT_CURSOR_LEAVE    0x08
#define PLATFORM_EVENT_CURSOR_ENTER    0x09

// =============================
// >> PLATFORM STRUCTS

typedef struct Platform_Event Platform_Event;
struct Platform_Event {
    s32 type;
    s32 key_index;
    s32 key_modifiers;
    u32 character;
    Vec2i mouse_pos;
    s32 scroll_delta;
};

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
    u32 event_count;
    Platform_Event events[1024];
};

static Platform_State *platform_state = 0;


// =============================
// >> PLATFORM PROCEDURES

// > Platform Specific
void platform_log(char *format, ...);
b32 platform_read_entire_file(char *file_name, Platform_File *result);
void * platform_reserve_memory(u64 size);
void platform_commit_memory(void *mem, u64 size);
void platform_release_memory(void *mem);
void platform_decommit(void *mem, u64 size);

// > Application Specific
void platform_push_event(Platform_Event event);


// =============================
// >> IMPLEMENTATION

#ifdef PLATFORM_IMPLEMENTATION
void platform_push_event(Platform_Event event) {
    if (platform_state->event_count < ArrayCount(platform_state->events)) {
        platform_state->events[platform_state->event_count] = event;
        platform_state->event_count += 1;
    }
}
#endif

#endif
