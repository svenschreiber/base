/* math.h - v0.1 - Sven A. Schreiber
 *
 * math.h is a single header file library for anything math related.
 * For now it provides simple vector math. It is part of and depends
 * on my C base-layer.
 *
 * To use this file simply define MATH_IMPL once at the start of
 * your project before including it. After that you can include it 
 * without defining MATH_IMPL as per usual.
 * 
 * Example:
 * ...
 * #define MATH_IMPL
 * #include "math.h"
 * ...
 */

#ifndef MATH_H
#define MATH_H

// +============+
// | DEFINTIONS |
// +============+

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

typedef struct mat4 mat4;
struct mat4 {
    f32 data[4][4];
};


// +===========+
// | INTERFACE |
// +===========+

vec3 v3_add_v3(vec3 a, vec3 b);
vec3 v3_sub_v3(vec3 a, vec3 b);
vec3 v3_mul_f32(vec3 a, f32 b);
f32 v3_length(vec3 a);
vec3 v3_normalize(vec3 a);
f32 v3_dot(vec3 a, vec3 b);
vec3 v3_cross(vec3 a, vec3 b);

vec4 v4_add_v4(vec4 a, vec4 b);
vec4 v4_sub_v4(vec4 a, vec4 b);
vec4 v4_mul_f32(vec4 a, f32 b);
f32 v4_dot(vec4 a, vec4 b);

mat4 m4_identity();
mat4 m4_mul_m4(mat4 a, mat4 b);
mat4 m4_mul_f32(mat4 a, f32 b);
vec4 m4_mul_v4(mat4 a, vec4 b);
mat4 m4_scale(mat4 a, vec3 scale);
mat4 m4_translate(mat4 a, vec3 translation);
mat4 m4_rotate(mat4 a, vec3 axis, f32 theta);


// +===============+
// | HELPER MACROS |
// +===============+

#define Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

#define vec2(...) (vec2){ __VA_ARGS__ }
#define vec3(...) (vec3){ __VA_ARGS__ }
#define vec4(...) (vec4){ __VA_ARGS__ }
#define ivec2(...) (ivec2){ __VA_ARGS__ }
#define ivec3(...) (ivec3){ __VA_ARGS__ }
#define ivec4(...) (ivec4){ __VA_ARGS__ }


// +================+
// | IMPLEMENTATION |
// +================+

#ifdef MATH_IMPL

vec3 v3_add_v3(vec3 a, vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3 v3_sub_v3(vec3 a, vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3 v3_mul_f32(vec3 a, f32 b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}

f32 v3_length(vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3 v3_normalize(vec3 a) {
    f32 l = v3_length(a);
    return vec3(a.x / l, a.y / l, a.z / l);
}

f32 v3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 v3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

mat4 m4_identity() {
    return (mat4) {
        {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }
    };
}

mat4 m4_mul_m4(mat4 a, mat4 b) {
    mat4 result = {0};
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            result.data[y][x] = (a.data[y][0] * b.data[0][x] +
                                 a.data[y][1] * b.data[1][x] +
                                 a.data[y][2] * b.data[2][x] +
                                 a.data[y][3] * b.data[3][x]);
        }
    }
    return result;
}

mat4 m4_mul_f32(mat4 a, f32 b) {
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            a.data[i][j] *= b;
        }
    }
    return a;
}

vec4 v4_add_v4(vec4 a, vec4 b) {
    return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

vec4 v4_sub_v4(vec4 a, vec4 b) {
    return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

vec4 v4_mul_f32(vec4 a, f32 b) {
    return vec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

f32 v4_dot(vec4 a, vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

vec4 m4_mul_v4(mat4 a, vec4 b) {
    vec4 result = {0};
    for (int i = 0; i < 4; ++i) {
        result.data[i] = (a.data[i][0] * b.data[0] +
                          a.data[i][1] * b.data[1] +
                          a.data[i][2] * b.data[2] +
                          a.data[i][3] * b.data[3]);
    }
    return result;
}

mat4 m4_scale(mat4 a, vec3 scale) {
    mat4 result = m4_identity();
    result.data[0][0] = scale.x;
    result.data[1][1] = scale.y;
    result.data[2][2] = scale.z;
    return result;
}

mat4 m4_translate(mat4 a, vec3 translation) {
    mat4 result = m4_identity();
    result.data[0][3] = translation.x;
    result.data[1][3] = translation.y;
    result.data[2][3] = translation.z;
    return result;
}

mat4 m4_rotate(mat4 a, vec3 axis, f32 theta) {
    mat4 result = m4_identity();
    result.data[0][0] = cosf(theta) + axis.x * axis.x * (1.0f - cosf(theta));
    result.data[0][1] = axis.x * axis.y * (1.0f - cosf(theta)) - axis.z * sinf(theta);
    result.data[0][2] = axis.x * axis.z * (1.0f - cosf(theta)) + axis.y * sinf(theta);

    result.data[1][0] = axis.y * axis.x * (1.0f - cosf(theta)) + axis.z * sinf(theta);
    result.data[1][1] = cosf(theta) + axis.y * axis.y * (1.0f - cosf(theta));
    result.data[1][2] = axis.y * axis.z * (1.0f - cosf(theta)) - axis.x * sinf(theta);

    result.data[2][0] = axis.z * axis.x * (1.0f - cosf(theta)) - axis.y * sinf(theta);
    result.data[2][1] = axis.z * axis.y * (1.0f - cosf(theta)) + axis.x * sinf(theta);
    result.data[2][2] = cosf(theta) + axis.z * axis.z * (1.0f - cosf(theta));
    
    return result;
}

#endif

#endif
