/* func_800083CC: build a 4x4 matrix from a translation and three basis vectors (built with -fno-schedule-insns). */
typedef float f32;

void func_800083CC(f32 m[4][4], f32 tx, f32 ty, f32 tz, f32 ax, f32 ay, f32 az,
                   f32 bx, f32 by, f32 bz, f32 cx, f32 cy, f32 cz) {
    m[0][0] = -cx;
    m[0][1] = -cy;
    m[0][2] = -cz;
    m[0][3] = 0.0f;
    m[1][0] = bx;
    m[1][1] = by;
    m[1][2] = bz;
    m[1][3] = 0.0f;
    m[2][0] = ax;
    m[2][1] = ay;
    m[2][2] = az;
    m[2][3] = 0.0f;
    m[3][0] = tx;
    m[3][1] = ty;
    m[3][2] = tz;
    m[3][3] = 1.0f;
}
