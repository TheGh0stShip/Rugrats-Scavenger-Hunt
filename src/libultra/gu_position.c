typedef signed int s32;
typedef float f32;

typedef struct Mtx {
    s32 m[4][4];
} Mtx;

extern f32 D_80056000;
extern f32 func_800475F0(f32 value);
extern f32 func_80046090(f32 value);
extern void func_800468F0(f32 source[4][4], Mtx *destination);

static void make_position(f32 mf[4][4], f32 roll, f32 pitch, f32 yaw,
                          f32 scale, f32 x, f32 y, f32 z)
{
    f32 sinRoll;
    f32 sinPitch;
    f32 sinYaw;
    f32 cosRoll;
    f32 cosPitch;
    f32 cosYaw;

    roll *= D_80056000;
    pitch *= D_80056000;
    yaw *= D_80056000;
    sinRoll = func_800475F0(roll);
    cosRoll = func_80046090(roll);
    sinPitch = func_800475F0(pitch);
    cosPitch = func_80046090(pitch);
    sinYaw = func_800475F0(yaw);
    cosYaw = func_80046090(yaw);

    mf[0][0] = (cosPitch * cosYaw) * scale;
    mf[0][1] = (cosPitch * sinYaw) * scale;
    mf[0][2] = (-sinPitch) * scale;
    mf[0][3] = 0.0f;
    mf[1][0] = (sinRoll * sinPitch * cosYaw - cosRoll * sinYaw) * scale;
    mf[1][1] = (sinRoll * sinPitch * sinYaw + cosRoll * cosYaw) * scale;
    mf[1][2] = (sinRoll * cosPitch) * scale;
    mf[1][3] = 0.0f;
    mf[2][0] = (cosRoll * sinPitch * cosYaw + sinRoll * sinYaw) * scale;
    mf[2][1] = (cosRoll * sinPitch * sinYaw - sinRoll * cosYaw) * scale;
    mf[2][2] = (cosRoll * cosPitch) * scale;
    mf[2][3] = 0.0f;
    mf[3][0] = x;
    mf[3][1] = y;
    mf[3][2] = z;
    mf[3][3] = 1.0f;
}

void func_80047A38(Mtx *m, f32 roll, f32 pitch, f32 yaw, f32 scale,
                   f32 x, f32 y, f32 z)
{
    f32 mf[4][4];

    make_position(mf, roll, pitch, yaw, scale, x, y, z);
    func_800468F0(mf, m);
}
