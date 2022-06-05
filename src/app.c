#include "base.h"
#define MATH_IMPL
#include "math.h"
#define PLATFORM_IMPL
#include "platform.h"
#define MEMORY_IMPL
#include "memory.h"
#define STRING_IMPL
#include "string.h"
#include "key_input.h"
#include "opengl.h"
#include "app.h"

static App_Data *app_data = 0;

static void app_process_events() {
    for (u32 i = 0; i < platform_state->event_count; ++i) {
        Platform_Event *event = &platform_state->events[i];
        switch (event->type) {

            case Platform_Event_Type_Key_Press: {
                platform_log("%s pressed!\n", get_key_name(event->key).str);
            } break;

            case Platform_Event_Type_Key_Release: {
                platform_log("%s released!\n", get_key_name(event->key).str);
            } break;

            case Platform_Event_Type_Character_Input: {
                platform_log("%c typed!\n", (char)event->character);
            } break;

            case Platform_Event_Type_Mouse_Press: {
                platform_log("%s pressed!\n", get_key_name(event->key).str);
            } break;

            case Platform_Event_Type_Mouse_Release: {
                platform_log("%s released!\n", get_key_name(event->key).str);
            } break;

            case Platform_Event_Type_Mouse_Move: {
                app_data->mouse_pos = event->mouse_pos;
            } break;

            case Platform_Event_Type_Cursor_Enter: {
                platform_log("cursor enter\n");
            } break;

            case Platform_Event_Type_Cursor_Leave: {
                platform_log("cursor leave\n");
            } break;
        }
    }
    platform_state->event_count = 0;
}

void app_init() {
    {
        Mem_Arena temp = mem_arena_init(GB(4));
        app_data = PushStruct(&temp, App_Data);
        app_data->arena = PushStruct(&temp, Mem_Arena);
        *app_data->arena = temp;
    }
    Mem_Arena *arena = app_data->arena;

    app_data->frame_arena = PushStruct(arena, Mem_Arena);
    *app_data->frame_arena = mem_arena_init(GB(1));

    platform_state->events = PushData(arena, Platform_Event, PLATFORM_MAX_EVENTS);

    load_gl_functions();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void app_update() {
    mem_arena_clear(app_data->frame_arena);

    app_process_events();

    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 0.5f, -0.5f, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f( 0.0f,  0.5f, 0.0f);
        
    glEnd();

    platform_swap_buffers();
}


// =============================
// >> PLATFORM SPECIFIC

#if defined(BUILD_WIN32)
#include "win32/win32_app.c"
#else
#error The specified platform is not yet supported. Make sure the correct define is set in the build file.
#endif
