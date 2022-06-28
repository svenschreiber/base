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
#define STB_TRUETYPE_IMPLEMENTATION
#include "ext/stb_truetype.h"
#define UI_IMPL
#include "ui.h"
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

static UI_State *ui = 0;

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

    UI_Font_Data font = ui_font_load(arena, "res/consolas.ttf", 18.0f);
    ui = ui_state_make(font); // TODO: load font

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (f32)platform_state->window_width, (f32)platform_state->window_height, 0.0f, 0.0f, 1.0f);
}

void render_triangle_gradient(f32 time) {
    f32 PI = 3.14159f;
    
    glBegin(GL_TRIANGLES);
    f32 w = (f32)platform_state->window_width;
    f32 h = (f32)platform_state->window_height;
    glColor3f(0.3f * sinf(time) + 0.7f, 0.0f, sinf(time + 0.7f * PI) + 0.7f);
    glVertex3f(w * 0.25f, h * 0.75f, 0.0f);
    glColor3f(0.3f * sinf(time + PI) + 0.7f, 0.0f, sinf(time + 0.3f * PI) + 0.7f);
    glVertex3f(w * 0.75f, h * 0.75f, 0.0f);
    glColor3f(0.3f * sinf(time + PI) + 0.7f, 0.0f, 0.3f * sinf(time + 0.8f * PI) + 0.7f);
    glVertex3f(w * 0.5f, h * 0.25f, 0.0f);
    glEnd();
}

void app_update() {
    mem_arena_clear(app_data->frame_arena);

    app_process_events();

    glClear(GL_COLOR_BUFFER_BIT);

    static f32 time = 0;
    time += platform_state->delta;
    render_triangle_gradient(time);
    
    ui_begin(ui, platform_state);
    UI_Box *box1 = ui_box_make(UI_Box_Flag_Fixed_Width | UI_Box_Flag_Fixed_Height | UI_Box_Flag_Draw_Background, Str("1"));
    box1->size[UI_Axis_X] = ui_pixel_size(50.0f);
    box1->size[UI_Axis_Y] = ui_pixel_size(30.0f);
    box1->style.background = vec4(0.25f * sinf(time) + 0.75f, 0.0f, 0.0f, 1.0f);

    UI_Box *box2 = ui_box_make(UI_Box_Flag_Fixed_Height | UI_Box_Flag_Draw_Background, Str("2"));
    box2->size[UI_Axis_X] = ui_parent_percent_size(0.2f);//ui_children_sum_size();
    box2->size[UI_Axis_Y] = ui_pixel_size(30.0f);
    box2->style.background = vec4(0.0f, 0.5f, 0.0f, 1.0f);

    ui_push_parent(box2);
    UI_Box *box4 = ui_box_make(UI_Box_Flag_Fixed_Width | UI_Box_Flag_Fixed_Height | UI_Box_Flag_Draw_Background, Str("4"));
    box4->size[UI_Axis_X] = ui_pixel_size(30.0f * sinf(time) + 30.0f);
    box4->size[UI_Axis_X].strictness = 0.0f;
    box4->size[UI_Axis_Y] = ui_pixel_size(15.0f);
    box4->style.background = vec4(1.0f, 1.0f, 1.0f, 1.0f);

    UI_Box *box5 = ui_box_make(UI_Box_Flag_Fixed_Width | UI_Box_Flag_Fixed_Height | UI_Box_Flag_Draw_Background, Str("5"));
    box5->size[UI_Axis_X] = ui_pixel_size(30.0f);
    box5->size[UI_Axis_X].strictness = 0.5f;
    box5->size[UI_Axis_Y] = ui_pixel_size(15.0f);
    box5->style.background = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    ui_pop_parent();

    UI_Box *box3 = ui_box_make(UI_Box_Flag_Fixed_Width | UI_Box_Flag_Fixed_Height | UI_Box_Flag_Draw_Background | UI_Box_Flag_Draw_Text, Str("Please click me!"));
    box3->size[UI_Axis_X] = ui_text_content_size(10.0f);
    box3->size[UI_Axis_Y] = ui_text_content_size(10.0f);
    box3->style.background = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    box3->style.text       = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    ui_end();

    ui_render(ui);

    platform_swap_buffers();
}


// =============================
// >> PLATFORM SPECIFIC

#if defined(BUILD_WIN32)
#include "win32/win32_app.c"
#else
#error The specified platform is not yet supported. Make sure the correct define is set in the build file.
#endif
