#ifndef APP_H
#define APP_H

typedef struct App_Data App_Data;
struct App_Data {
    Mem_Arena *arena;
    Mem_Arena *frame_arena;
    ivec2 mouse_pos; // Maybe make this a float vec2?
};

#endif
