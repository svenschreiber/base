/* platform.h - v0.1 - Sven A. Schreiber
 *
 * platform.h is a single header file operating system
 * abstraction. It offers a platform independent interface
 * for OS functionallity, such as loading a file, handling
 * window/input events,...
 * 
 * To use this file simply define PLATFORM_IMPL once at the start of
 * your project before including it. After that you can include it 
 * without defining PLATFORM_IMPL as per usual.
 * 
 * Example:
 * ...
 * #define PLATFORM_IMPL
 * #include "platform.h"
 * ...
 */

#ifndef PLATFORM_H
#define PLATFORM_H

// +============+
// | DEFINTIONS |
// +============+

#define PLATFORM_DEFAULT_WINDOW_WIDTH  1280
#define PLATFORM_DEFAULT_WINDOW_HEIGHT 720


#define PLATFORM_MAX_EVENTS 1024

typedef enum Platform_Event_Type Platform_Event_Type;
enum Platform_Event_Type {
    Platform_Event_Type_None,
    Platform_Event_Type_Key_Press,
    Platform_Event_Type_Key_Release,
    Platform_Event_Type_Character_Input,
    Platform_Event_Type_Mouse_Press,
    Platform_Event_Type_Mouse_Release,
    Platform_Event_Type_Mouse_Move,
    Platform_Event_Type_Mouse_Scroll,
    Platform_Event_Type_Cursor_Leave,
    Platform_Event_Type_Cursor_Enter
};

typedef enum Key_Modifiers Key_Modifiers;
typedef enum Key Key;

typedef struct Platform_Event Platform_Event;
struct Platform_Event {
    Platform_Event_Type type;
    Key key;
    Key_Modifiers key_modifiers;
    u32 character;
    ivec2 mouse_pos;
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
    f32 delta;
    u32 event_count;
    Platform_Event *events;
};


// +===========+
// | INTERFACE |
// +===========+

static Platform_State *platform_state = 0;

void platform_log(char *format, ...);
b32 platform_read_entire_file(char *file_name, Platform_File *result);
void *platform_reserve_memory(u64 size);
void platform_commit_memory(void *mem, u64 size);
void platform_release_memory(void *mem);
void platform_decommit(void *mem, u64 size);
void platform_swap_buffers();
void *platform_get_gl_proc_address(char *function_name);

void platform_push_event(Platform_Event event);


// +================+
// | IMPLEMENTATION |
// +================+

#ifdef PLATFORM_IMPL

void platform_push_event(Platform_Event event) {
    if (platform_state->event_count < PLATFORM_MAX_EVENTS) {
        platform_state->events[platform_state->event_count] = event;
        platform_state->event_count += 1;
    }
}

#endif

#endif
