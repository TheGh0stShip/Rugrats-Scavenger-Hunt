typedef int s32;
typedef float f32;

extern void func_800510F0(const char *fmt, ...);

s32 func_8000857C(f32 m[4][4], f32 px, f32 py, f32 pz) {
    f32 v[4];
    f32 x, y, z;
    s32 code = 0;

    v[0] = px * m[0][0] + py * m[1][0] + pz * m[2][0] + m[3][0];
    v[1] = px * m[0][1] + py * m[1][1] + pz * m[2][1] + m[3][1];
    v[2] = px * m[0][2] + py * m[1][2] + pz * m[2][2] + m[3][2];
    v[3] = px * m[0][3] + py * m[1][3] + pz * m[2][3] + m[3][3];

    if (v[3] == 0.0f) {
        func_800510F0("DIVIDE BY ZERO!!!\n");
        return 0;
    }
    if (v[3] != 1.0f) {
        x = v[0] / v[3];
        y = v[1] / v[3];
        z = v[2] / v[3];
    }
    if (0.0f <= v[3]) {
        if (1.0f < x) code = 1;
        if (x < -1.0f) code |= 2;
        if (1.0f < y) code |= 4;
        if (y < -1.0f) code |= 8;
        if (1.0f < z) code |= 0x10;
        if (z < -1.0f) code |= 0x20;
    } else {
        if (-1.0f < x) code = 2;
        if (x < 1.0f) code |= 1;
        if (1.0f < y) code |= 8;
        if (y < -1.0f) code |= 4;
        code |= 0x20;
    }
    return code;
}
