static vec3 v3_add_v3(vec3 a, vec3 b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static vec3 v3_sub_v3(vec3 a, vec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static vec3 v3_mul_f32(vec3 a, f32 b) {
    return vec3(a.x * b, a.y * b, a.z * b);
}

static f32 v3_length(vec3 a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

static vec3 v3_normalize(vec3 a) {
    f32 l = v3_length(a);
    return vec3(a.x / l, a.y / l, a.z / l);
}

static f32 v3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static vec3 v3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static mat4 m4_identity() {
    return (mat4) {
        {
            {1.0f, 0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f}
        }
    };
}

static mat4 m4_mul_m4(mat4 a, mat4 b) {
    mat4 result = {0};
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            result.data[i][j] = (a.data[0][j] * b.data[i][0] +
                                 a.data[1][j] * b.data[i][1] +
                                 a.data[2][j] * b.data[i][2] +
                                 a.data[3][j] * b.data[i][3]);
        }
    }
    return result;
}

static mat4 m4_mul_f32(mat4 a, f32 b) {
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            a.data[i][j] *= b;
        }
    }
    return a;
}

static vec4 v4_add_v4(vec4 a, vec4 b) {
    return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

static vec4 v4_sub_v4(vec4 a, vec4 b) {
    return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

static vec4 v4_mul_f32(vec4 a, f32 b) {
    return vec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

static f32 v4_dot(vec4 a, vec4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

static vec4 v4_mul_m4(vec4 a, mat4 b) {
    vec4 result = {0};
    for (int i = 0; i < 4; ++i) {
        result.data[i] = (a.data[0] * b.data[0][i] +
                          a.data[1] * b.data[1][i] +
                          a.data[2] * b.data[2][i] +
                          a.data[3] * b.data[3][i]);
    }
    return result;
}

