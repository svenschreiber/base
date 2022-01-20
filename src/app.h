#ifndef APP_H
#define APP_H

typedef struct App_Data App_Data;
struct App_Data {
    Mem_Arena *arena;
    Vec2i mouse_pos;
};

#endif
