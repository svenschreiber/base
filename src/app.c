#include "base.h"
#include "math.h"
#define PLATFORM_IMPLEMENTATION
#include "platform.h"
#include "key_input.h"
#include "memory_arena.h"

static void app_process_events() {
    for (u32 i = 0; i < platform_state->event_count; ++i) {
        Platform_Event *event = &platform_state->events[i];
        switch (event->type) {

            case PLATFORM_EVENT_KEY_PRESS: {
                platform_log("%s pressed!\n", get_key_name(event->key_index).str);
            } break;

            case PLATFORM_EVENT_KEY_RELEASE: {
                platform_log("%s released!\n", get_key_name(event->key_index).str);
            } break;

            case PLATFORM_EVENT_CHARACTER_INPUT: {
                platform_log("%c typed!\n", (char)event->character);
            } break;

            case PLATFORM_EVENT_MOUSE_PRESS: {
                platform_log("%s pressed!\n", get_key_name(event->key_index).str);
            } break;

            case PLATFORM_EVENT_MOUSE_RELEASE: {
                platform_log("%s released!\n", get_key_name(event->key_index).str);
            } break;

            case PLATFORM_EVENT_MOUSE_MOVE: {
                
            } break;

            case PLATFORM_EVENT_CURSOR_ENTER: {
                platform_log("cursor enter\n");
            } break;

            case PLATFORM_EVENT_CURSOR_LEAVE: {
                platform_log("cursor leave\n");
            } break;
        }
    }
    platform_state->event_count = 0;
}

void app_init() {

}

void app_update() {
    app_process_events();
}


// =============================
// >> PLATFORM SPECIFIC

#if defined(BUILD_WIN32)
#include "win32/win32_app.c"
#else
#error The specified platform is not yet supported. Make sure the correct define is set in the build file.
#endif
