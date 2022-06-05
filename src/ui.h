/* ui.h - v0.1 - Sven A. Schreiber
 *
 * ui.h is a single header file library for creating immediate
 * mode user interfaces. It is part of and depends on my C base-
 * layer.
 * 
 * This UI library is inspired and heavily influenced by Ryan Fleury's
 * series of blog posts about UI design:
 * https://ryanfleury.substack.com/p/ui-part-1-the-interaction-medium
 * 
 * To use this file simply define UI_IMPL once at the start of
 * your project before including it. After that you can include it 
 * without defining UI_IMPL as per usual.
 * 
 * Example:
 * ...
 * #define UI_IMPL
 * #include "ui.h"
 * ...
 */

#ifndef UI_H
#define UI_H

// +============+
// | DEFINTIONS |
// +============+

typedef enum UI_Axis {
    UI_Axis_X,
    UI_Axis_Y,
    UI_Axis_Count
} UI_Axis;

typedef struct UI_Rect UI_Rect;
struct UI_Rect {
    vec2 p0;
    vec2 p1;
};

typedef enum UI_Size_Kind {
    UI_Size_Kind_Null,
    UI_Size_Kind_Pixels,
    UI_Size_Kind_Text_Content,
    UI_Size_Kind_Children_Sum,
    UI_Size_Kind_Parent_Percent
} UI_Size_Kind;

typedef struct UI_Size UI_Size;
struct UI_Size {
    UI_Size_Kind kind;
    f32 strictness;
    f32 value;
};

typedef enum UI_Box_Flags {
    UI_Box_Flag_Clickable           = (1 << 0),
    // ...
    UI_Box_Flag_Fixed_Width         = (1 << 1),
    UI_Box_Flag_Fixed_Height        = (1 << 2),
    UI_Box_Flag_Floating_X          = (1 << 3),
    UI_Box_Flag_Floating_Y          = (1 << 4),
    UI_Box_Flag_Allow_Overflow_X    = (1 << 5),
    UI_Box_Flag_Allow_Overflow_Y    = (1 << 6),
    UI_Box_Flag_Clip                = (1 << 7),
    UI_Box_Flag_Draw_Background     = (1 << 8)
} UI_Box_Flags;

typedef struct UI_Box_Style UI_Box_Style;
struct UI_Box_Style {
    vec4 background;
    vec4 border;
    vec4 text;
};

typedef struct UI_Box UI_Box;
struct UI_Box {
    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;    

    UI_Axis child_layout_axis;
    UI_Size size[UI_Axis_Count];
    vec2 fixed_size;
    vec2 fixed_pos;
    vec2 min_size;
    vec2 view_bounds;
    
    UI_Box_Flags flags;
    String text;
    UI_Box_Style style;

    UI_Rect rect;
};

typedef struct UI_State UI_State;
struct UI_State {
    Mem_Arena arena;
    Mem_Arena frame_arena;
    
    UI_Box *root;
    UI_Box *current_parent;
};


// +===========+
// | INTERFACE |
// +===========+

UI_Box *ui_box_make();
UI_Size ui_pixel_size(f32 pixels);
UI_Size ui_parent_percent_size(f32 percent);
UI_Size ui_children_sum_size();
void ui_render_rect(UI_Rect *rect);
void ui_set_arena(Mem_Arena *arena);
void ui_render();
UI_State *ui_state_make();
void ui_begin(UI_State *state, Platform_State *p_state);
void ui_end();
void ui_push_parent(UI_Box *box);
void ui_pop_parent();
void ui_render_tree_recursive(UI_Box *box);
void ui_layout_independent_sizes(UI_Box *box, UI_Axis axis);
void ui_layout_upwards_dependent(UI_Box *box, UI_Axis axis);
void ui_layout_downwards_dependent(UI_Box *box, UI_Axis axis);
void ui_layout_enforce_constraints(UI_Box *box, UI_Axis axis);
void ui_layout_position(UI_Box *box, UI_Axis axis);


// +================+
// | IMPLEMENTATION |
// +================+

#ifdef UI_IMPL

static UI_State *global_ui_state = 0;

void ui_push_parent(UI_Box *box) {
    global_ui_state->current_parent = box;
}

void ui_pop_parent() {
    UI_Box *parent = global_ui_state->current_parent;
    if (parent->parent) global_ui_state->current_parent = parent->parent;
}

void ui_begin(UI_State *state, Platform_State *p_state) {
    global_ui_state = state;

    mem_arena_clear(&global_ui_state->frame_arena);
    UI_Box *root = PushStructZero(&global_ui_state->frame_arena, UI_Box);
    root->fixed_size.data[UI_Axis_X] = (f32)platform_state->window_width;
    root->fixed_size.data[UI_Axis_Y] = (f32)platform_state->window_height;
    root->flags |= UI_Box_Flag_Fixed_Width;
    root->flags |= UI_Box_Flag_Fixed_Height;
    root->child_layout_axis = UI_Axis_X;
    //root->text = push_string(&global_ui_state->frame_arena, "R");
    global_ui_state->root = root;
    global_ui_state->current_parent = root;
}

void ui_end() {
    // setup hash table
    // ...

    for(UI_Axis axis = (UI_Axis)0; axis < UI_Axis_Count; axis = (UI_Axis)(axis + 1)) {
        ui_layout_independent_sizes(global_ui_state->root, axis);
        ui_layout_upwards_dependent(global_ui_state->root, axis);
        ui_layout_downwards_dependent(global_ui_state->root, axis);
        ui_layout_enforce_constraints(global_ui_state->root, axis);
        ui_layout_position(global_ui_state->root, axis);
    }
}

UI_Size ui_pixel_size(f32 pixels) {
    return (UI_Size){UI_Size_Kind_Pixels, 1.0f, pixels};
}

UI_Size ui_parent_percent_size(f32 percent) {
    return (UI_Size){UI_Size_Kind_Parent_Percent, 1.0f, percent};
}

UI_Size ui_children_sum_size() {
    return (UI_Size){UI_Size_Kind_Children_Sum, 1.0f, 0.0f};
}

UI_State *ui_state_make() {
    Mem_Arena arena = mem_arena_init(GB(64));
    UI_State *state = PushStruct(&arena, UI_State);
    state->arena = arena;
    state->frame_arena = mem_arena_init(GB(1));

    return state;
}

UI_Box *ui_box_make(UI_Box_Flags flags, String text) {
    UI_Box *parent = global_ui_state->current_parent;
    Mem_Arena *frame_arena = &global_ui_state->frame_arena;
    UI_Box *box = PushStructZero(&global_ui_state->frame_arena, UI_Box);

    box->parent = parent;
    box->flags = flags;
    box->text = str_copy(frame_arena, text);
    
    DLL_PushBack(parent, box);
    return box;
}

void ui_layout_independent_sizes(UI_Box *box, UI_Axis axis) {
    switch (box->size[axis].kind) {
        case UI_Size_Kind_Pixels: {
            box->fixed_size.data[axis] = box->size[axis].value;
        } break;

            //case UI_Size_Kind_Text_Content:
    }

    for (UI_Box *child = box->first; child; child = child->next) {
        ui_layout_independent_sizes(child, axis);
    }
}

void ui_layout_upwards_dependent(UI_Box *box, UI_Axis axis) {
    switch (box->size[axis].kind) {
        default: break;

        case UI_Size_Kind_Parent_Percent: {
            UI_Box *fixed_parent = 0;
            for (UI_Box *parent = box->parent; parent; parent = parent->parent) {
                if (parent->flags & (UI_Box_Flag_Fixed_Width << axis)) {
                    fixed_parent = parent;
                    break;
                }
            }

            f32 size = fixed_parent->fixed_size.data[axis] * box->size[axis].value;
            box->fixed_size.data[axis] = size;
            box->flags |= (UI_Box_Flag_Fixed_Width << axis);
        } break;
    }

    for (UI_Box *child = box->first; child; child = child->next) {
        ui_layout_upwards_dependent(child, axis);
    }
}

void ui_layout_downwards_dependent(UI_Box *box, UI_Axis axis) {
    for (UI_Box *child = box->first; child; child = child->next) {
        ui_layout_downwards_dependent(child, axis);
    }

    switch (box->size[axis].kind) {
        default: break;

        case UI_Size_Kind_Children_Sum: {
            f32 sum = 0;
            for (UI_Box *child = box->first; child; child = child->next) {
                if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
                    if (axis == box->child_layout_axis) {
                        sum += child->fixed_size.data[axis];
                    } else {
                        sum = max(sum, child->fixed_size.data[axis]);
                    }
                }
            }

            box->fixed_size.data[axis] = sum;
        } break;
    }
}


u32 ui_count_childs(UI_Box *box) {
    u32 childs = 0;
    for (UI_Box *child = box->first; child; child = child->next) {
        ++childs;
    }
    return childs;
}

void ui_layout_enforce_constraints(UI_Box *box, UI_Axis axis) {
    if (axis != box->child_layout_axis && !(box->flags & (UI_Box_Flag_Allow_Overflow_X << axis))) {
        f32 allowed_size = box->fixed_size.data[axis];
        for (UI_Box *child = box->first; child; child = child->next) {
            if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
                f32 child_size = child->fixed_size.data[axis];
                f32 overflow = child_size - allowed_size;
                f32 max_fixup = child_size - child->min_size.data[axis];
                f32 fixup = Clamp(0, overflow, max_fixup);
                if (fixup > 0) {
                    child->fixed_size.data[axis] -= fixup;
                }
            }
        }
    }

    if (axis == box->child_layout_axis && !(box->flags & (UI_Box_Flag_Allow_Overflow_X << axis))) {
        f32 total_allowed_size = box->fixed_size.data[axis];
        f32 total_size = 0;
        f32 total_weighted_size = 0;
        for (UI_Box *child = box->first; child; child = child->next) {
            if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
                total_size += child->fixed_size.data[axis];
                total_weighted_size += child->fixed_size.data[axis] * (1 - child->size[axis].strictness);
            }
        }

        f32 overflow = total_size - total_allowed_size;
        if (overflow > 0) {
            f32 child_fixup_sum = 0;
            f32 *child_fixups = PushDataZero(&global_ui_state->frame_arena, f32, ui_count_childs(box));
            {
                u64 child_index = 0;
                for (UI_Box *child = box->first; child; child = child->next, ++child_index) {
                    if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
                        f32 current_fixup_size = child->fixed_size.data[axis] * (1 - child->size[axis].strictness);
                        if (child->fixed_size.data[axis] - current_fixup_size < child->min_size.data[axis]) {
                            current_fixup_size = child->fixed_size.data[axis] - child->min_size.data[axis];
                        }
                        current_fixup_size = max(0, current_fixup_size);
                        child_fixups[child_index] = current_fixup_size;
                        child_fixup_sum += current_fixup_size;
                    }
                }
            }

            {
                u64 child_index = 0;
                for (UI_Box *child = box->first; child; child = child->next, ++child_index) {
                    if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
                        f32 fixup_percent = overflow / total_weighted_size;
                        fixup_percent = Clamp(0, fixup_percent, 1);
                        child->fixed_size.data[axis] -= child_fixups[child_index] * fixup_percent;
                    }
                }
            }
        }
    }

    for (UI_Box *child = box->first; child; child = child->next) {
        ui_layout_enforce_constraints(child, axis);
    }
}

void ui_layout_position(UI_Box *box, UI_Axis axis) {
    f32 current_pos = 0;
    f32 bounds = 0;

    for (UI_Box *child = box->first; child; child = child->next) {
        
        if (!(child->flags & (UI_Box_Flag_Floating_X << axis))) {
            child->fixed_pos.data[axis] = current_pos;
            if (box->child_layout_axis == axis) {
                current_pos += child->fixed_size.data[axis];
                bounds += child->fixed_size.data[axis];
            } else {
                bounds = max(bounds, child->fixed_size.data[axis]);
            }
        }

        child->rect.p0.data[axis] = box->rect.p0.data[axis] + child->fixed_pos.data[axis];
        child->rect.p1.data[axis] = child->rect.p0.data[axis] + child->fixed_size.data[axis];

    }

    box->view_bounds.data[axis] = bounds;

    for (UI_Box *child = box->first; child; child = child->next) {
        ui_layout_position(child, axis);
    }
}

void ui_render_box(UI_Box *box) {
    UI_Rect *rect = &box->rect;
    UI_Box_Style *style = &box->style;
    vec4 bg = style->background;

    glBegin(GL_TRIANGLES);
    glColor3f(bg.r, bg.g, bg.b);
    glVertex3f(rect->p0.x, rect->p0.y, 0.0f);
    glVertex3f(rect->p0.x, rect->p1.y, 0.0f);
    glVertex3f(rect->p1.x, rect->p1.y, 0.0f);

    glVertex3f(rect->p0.x, rect->p0.y, 0.0f);
    glVertex3f(rect->p1.x, rect->p1.y, 0.0f);
    glVertex3f(rect->p1.x, rect->p0.y, 0.0f);
    glEnd();
}

// render the tree by rendering next boxes first (on same layer)
// then working your way back while rendering children box layers
// recursively.
// TODO: Should profile this.
void ui_render_tree_recursive(UI_Box *box) {
    ui_render_box(box);
    //platform_log("%c\n", box->text.str[0]);
    if (box->next) {
        ui_render_tree_recursive(box->next);
    } else {
        UI_Box *current = box;
        while (!current->first) {
            if (current->prev) {
                current = current->prev;
            } else {
                if (current->parent) {
                    if (current->parent->prev) {
                        current = current->parent->prev;
                    } else {
                        while (current->parent && !current->parent->prev) {
                            current = current->parent;
                        }
                        if (current->parent) {
                            current = current->parent->prev;
                        } else {
                            return;
                        }
                    }
                }
            }
        }
        ui_render_tree_recursive(current->first);
    }    
}

void ui_render(UI_State *state) {
    //platform_log("TREE BEGIN\n");
    ui_render_tree_recursive(state->root);
}

#endif
#endif
