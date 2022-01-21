#ifndef MATH_H
#define MATH_H

typedef union vec2 vec2;
union vec2 {
    struct {
        f32 x;
        f32 y;
    };
    f32 data[2];
};

typedef union vec3 vec3;
union vec3 {
    struct {
        f32 x;
        f32 y;
        f32 z;
    };

    struct {
        f32 r;
        f32 g;
        f32 b;
    };
    
    f32 data[3];
};

typedef union vec4 vec4;
union vec4 {
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };

    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    
    f32 data[4];
};

typedef union ivec2 ivec2;
union ivec2 {
    struct {
        s32 x;
        s32 y;
    };
    s32 data[2];
};

typedef union ivec3 ivec3;
union ivec3 {
    struct {
        s32 x;
        s32 y;
        s32 z;
    };

    struct {
        s32 r;
        s32 g;
        s32 b;
    };
    
    s32 data[3];
};

typedef union ivec4 ivec4;
union ivec4 {
    struct {
        s32 x;
        s32 y;
        s32 z;
        s32 w;
    };

    struct {
        s32 r;
        s32 g;
        s32 b;
        s32 a;
    };
    
    s32 data[4];
};

#define vec2(...) (vec2){ __VA_ARGS__ }
#define vec3(...) (vec3){ __VA_ARGS__ }
#define vec4(...) (vec4){ __VA_ARGS__ }
#define ivec2(...) (ivec2){ __VA_ARGS__ }
#define ivec3(...) (ivec3){ __VA_ARGS__ }
#define ivec4(...) (ivec4){ __VA_ARGS__ }

typedef struct mat4 mat4;
struct mat4 {
    f32 data[4][4];
};

#endif
