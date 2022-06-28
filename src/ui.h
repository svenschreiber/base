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

typedef struct UI_Key UI_Key;
struct UI_Key {
    u32 hash;
};

typedef enum UI_Axis UI_Axis;
enum UI_Axis{
    UI_Axis_X,
    UI_Axis_Y,
    UI_Axis_Count
};

typedef struct UI_Rect UI_Rect;
struct UI_Rect {
    vec2 p0;
    vec2 p1;
};

typedef enum UI_Size_Kind UI_Size_Kind;
enum UI_Size_Kind {
    UI_Size_Kind_Null,
    UI_Size_Kind_Pixels,
    UI_Size_Kind_Text_Content,
    UI_Size_Kind_Children_Sum,
    UI_Size_Kind_Parent_Percent
};

typedef struct UI_Size UI_Size;
struct UI_Size {
    UI_Size_Kind kind;
    f32 strictness;
    f32 value;
};

typedef enum UI_Box_Flags UI_Box_Flags;
enum UI_Box_Flags {
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
};

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

    UI_Key key;
    UI_Box *hash_next;
    UI_Box *hash_prev;
    u64 frame_created;

    UI_Rect rect;
};

// TODO: move this in its own file
#define HASH_TABLE_MAX 2048

typedef struct UI_Hash_Table_Bucket UI_Hash_Table_Bucket;
struct UI_Hash_Table_Bucket {
    UI_Box *first;
    UI_Box *last;
};

typedef struct UI_Hash_Table UI_Hash_Table;
struct UI_Hash_Table {
    UI_Hash_Table_Bucket buckets[HASH_TABLE_MAX];
    u32 num_buckets;
};

typedef struct UI_Font_Data UI_Font_Data;
struct UI_Font_Data {
    stbtt_packedchar *char_data;
    u32 bm_width;
    u32 bm_height;
    u32 texture_id;
    f32 max_advance;
    f32 max_ascent;
    f32 max_descent;
    f32 max_height;
};

typedef struct UI_State UI_State;
struct UI_State {
    Mem_Arena arena;
    Mem_Arena frame_arena[2];
    UI_Font_Data font;

    UI_Box *root;
    UI_Box *current_parent;
    UI_Hash_Table hash_table;
    u64 current_frame;
};


// +===========+
// | INTERFACE |
// +===========+

UI_Box *ui_box_make();
UI_Size ui_pixel_size(f32 pixels);
UI_Size ui_parent_percent_size(f32 percent);
UI_Size ui_children_sum_size();
UI_Size ui_text_content_size();
void ui_render_rect(UI_Rect *rect);
void ui_set_arena(Mem_Arena *arena);
void ui_render();
UI_State *ui_state_make(UI_Font_Data font);
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
UI_Key ui_key_from_string(Mem_Arena *arena, String str);
void ui_hash_table_put(UI_Hash_Table *table, UI_Box *box);
void ui_hash_table_remove(UI_Hash_Table *table, UI_Box *box);
Mem_Arena *ui_frame_arena();
UI_Font_Data ui_font_load(Mem_Arena *arena, char *font_path, f32 font_size);

// +================+
// | IMPLEMENTATION |
// +================+

#ifdef UI_IMPL

static UI_State *global_ui_state = 0;

// TODO: Move this hashing stuff to its own file.
// https://web.mit.edu/freebsd/head/sys/libkern/crc32.c
static const u32 crc32_lookup_table[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

u32 crc32_hash(String s) {
    u8 *p = (u8*)s.str;
    u64 size = s.size;

    u32 crc = 0xffffffff;
    while (size) {
        crc = (crc >> 8) ^ crc32_lookup_table[(crc & 0xFF) ^ p[0]];
        --size;
        ++p;
    }
    return ~crc;
}

UI_Key ui_key_from_string(Mem_Arena *arena, String str) {
    String_List list = str_split(arena, str, Str("###"));
    if (list.num_nodes > 1) {
        str = list.first->next->string;
    }
    UI_Key result = {0};
    result.hash = crc32_hash(str);
    return result;
}

UI_Hash_Table_Bucket *ui_hash_table_bucket_from_box(UI_Hash_Table *table, UI_Box *box) {
    UI_Key key = box->key;
    u32 index = key.hash % table->num_buckets;
    return &table->buckets[index];
}

void ui_hash_table_put(UI_Hash_Table *table, UI_Box *box) {
    UI_Hash_Table_Bucket *bucket = ui_hash_table_bucket_from_box(table, box);
    Custom_DLL_PushBack(bucket, box, first, last, hash_next, hash_prev);
}

void ui_hash_table_remove(UI_Hash_Table *table, UI_Box *box) {
    UI_Hash_Table_Bucket *bucket = ui_hash_table_bucket_from_box(table, box);
    Custom_DLL_Remove(bucket, box, first, last, hash_next, hash_prev);
}

UI_Font_Data ui_font_load(Mem_Arena *arena, char *font_path, f32 font_size) {
    UI_Font_Data font = {0};

    Platform_File font_file;
    platform_read_entire_file(font_path, &font_file);

    u32 num_chars  = 512; // TODO: need more?
    font.char_data = PushData(arena, stbtt_packedchar, num_chars);
    font.bm_width  = 1024;
    font.bm_height = 512;
    
    u8 *bitmap = PushData(arena, u8, font.bm_width * font.bm_height);
    f32 scaled_font_size = STBTT_POINT_SIZE(font_size);
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, bitmap, font.bm_width, font.bm_height, 0, 1, 0);
    stbtt_PackSetOversampling(&pc, 2, 2); // @Hardcode
    stbtt_PackFontRange(&pc, font_file.data, 0, scaled_font_size, 32, num_chars - 32, font.char_data + 32);
    glGenTextures(1, &font.texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font.texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font.bm_width, font.bm_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbtt_fontinfo font_info;
    stbtt_InitFont(&font_info, font_file.data, 0);
    s32 ascent, descent, line_gap;
    s32 advance, lsb;
    stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
    stbtt_GetGlyphHMetrics(&font_info, (s32)'a', &advance, &lsb); // @Hardcode: only works for monospaced fonts
    f32 scale_factor = stbtt_ScaleForMappingEmToPixels(&font_info, font_size);

    platform_release_memory(font_file.data);

    font.max_advance = scale_factor * advance;
    font.max_ascent  = scale_factor * (ascent + line_gap);
    font.max_descent = scale_factor * descent;
    font.max_height  = scale_factor * (ascent - descent + line_gap);

    return font;
}

void ui_push_parent(UI_Box *box) {
    global_ui_state->current_parent = box;
}

void ui_pop_parent() {
    UI_Box *parent = global_ui_state->current_parent;
    if (parent->parent) global_ui_state->current_parent = parent->parent;
}

void ui_begin(UI_State *state, Platform_State *p_state) {
    global_ui_state = state;

    mem_arena_clear(ui_frame_arena());
    UI_Box *root = PushStructZero(ui_frame_arena(), UI_Box);
    root->fixed_size.data[UI_Axis_X] = (f32)platform_state->window_width;
    root->fixed_size.data[UI_Axis_Y] = (f32)platform_state->window_height;
    root->flags |= UI_Box_Flag_Fixed_Width;
    root->flags |= UI_Box_Flag_Fixed_Height;
    root->child_layout_axis = UI_Axis_X;
    root->text = str_pushf(ui_frame_arena(), "###%p", root);
    global_ui_state->root = root;
    global_ui_state->current_parent = root;
}

void ui_end() {
    UI_State *state = global_ui_state;
    UI_Hash_Table *hash_table = &state->hash_table;

    for (u32 i = 0; i < hash_table->num_buckets; ++i) {
        UI_Hash_Table_Bucket *bucket = &hash_table->buckets[i];
        for (UI_Box *box = bucket->first; box; box = box->hash_next) {
            if (box->frame_created < state->current_frame) {
                ui_hash_table_remove(hash_table, box);
            }
        }
    }

    for(UI_Axis axis = (UI_Axis)0; axis < UI_Axis_Count; axis = (UI_Axis)(axis + 1)) {
        ui_layout_independent_sizes(global_ui_state->root, axis);
        ui_layout_upwards_dependent(global_ui_state->root, axis);
        ui_layout_downwards_dependent(global_ui_state->root, axis);
        ui_layout_enforce_constraints(global_ui_state->root, axis);
        ui_layout_position(global_ui_state->root, axis);
    }

    global_ui_state->current_frame += 1;
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

UI_Size ui_text_content_size(f32 padding) {
    return (UI_Size){UI_Size_Kind_Text_Content, 1.0f, padding};
}

Mem_Arena *ui_frame_arena() {
    return &global_ui_state->frame_arena[global_ui_state->current_frame % 2];
}

UI_State *ui_state_make(UI_Font_Data font) {
    Mem_Arena arena = mem_arena_init(GB(64));
    UI_State *state = PushStructZero(&arena, UI_State);
    state->arena = arena;
    state->frame_arena[0] = mem_arena_init(GB(1));
    state->frame_arena[1] = mem_arena_init(GB(1));
    state->font = font;
    state->hash_table.num_buckets = HASH_TABLE_MAX;

    return state;
}

UI_Box *ui_box_make(UI_Box_Flags flags, String text) {
    UI_Box *parent = global_ui_state->current_parent;
    UI_Box *box = PushStructZero(ui_frame_arena(), UI_Box);

    box->parent = parent;
    box->flags = flags;
    box->text = str_copy(ui_frame_arena(), text);

    box->key = ui_key_from_string(ui_frame_arena(), box->text);
    box->frame_created = global_ui_state->current_frame;
    ui_hash_table_put(&global_ui_state->hash_table, box);

    DLL_PushBack(parent, box);
    return box;
}

void ui_layout_independent_sizes(UI_Box *box, UI_Axis axis) {
    switch (box->size[axis].kind) {
        case UI_Size_Kind_Pixels: {
            box->fixed_size.data[axis] = box->size[axis].value;
        } break;

        case UI_Size_Kind_Text_Content: {
            f32 padding = box->size[axis].value;
            if (axis == UI_Axis_X) {
                box->fixed_size.data[axis] = box->text.size * global_ui_state->font.max_advance + padding;
            } else if (axis == UI_Axis_Y) {
                box->fixed_size.data[axis] = global_ui_state->font.max_height + padding;
            }
        } break;
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
            f32 *child_fixups = PushDataZero(ui_frame_arena(), f32, ui_count_childs(box));
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
