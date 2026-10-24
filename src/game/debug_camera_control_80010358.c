/* func_80010358: free camera control from controller input. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef f32 Mtx_f[4][4];

typedef struct { f32 x, y, z; } Vec3f;

struct Cam_80010358 {
    u8 pad_00[0x58];
    f32 unk_58;
    f32 unk_5C;
    f32 unk_60;
    u8 pad_64[0xFC - 0x64];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
    u8 pad_108[0x114 - 0x108];
    Vec3f pos;
};

struct Player_80005544 {
    u8 pad_00[0x11C];
    s32 unk_11C;
};

typedef struct {
    u16 cur;
    u16 raw;
    u16 pressed;
    s8 stick_x;
    s8 stick_y;
    u16 unk8;
} Ctrl_80058B58;

extern Ctrl_80058B58 D_80058B58[];
extern struct Player_80005544 *func_80005544(void);
extern s32 func_8000DD60(void);
extern s32 func_8000DD74(void);
extern void func_80047790(Mtx_f m, f32 x, f32 y, f32 z);
extern void func_80047230(Mtx_f m, f32 a, f32 x, f32 y, f32 z);
extern void func_80046B70(Mtx_f a, Mtx_f b, Mtx_f out);
extern void func_80046C4C(Mtx_f m, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz);
extern void func_8000FB08(struct Cam_80010358 *cam, Vec3f pos);

void func_80010358(struct Cam_80010358 *cam) {
    Mtx_f rot;
    Mtx_f m;
    Mtx_f trans;
    s32 idx = func_80005544()->unk_11C;
    u32 btn = D_80058B58[idx].raw;
    s32 sx = D_80058B58[idx].stick_x;
    s32 sy = D_80058B58[idx].stick_y;
    f32 lim;
    f32 zero;
    Vec3f pad;

    if (sx != 0 || sy != 0) {
        zero = 0.0f;
        func_80047790(trans, cam->pos.x, zero, cam->pos.z);
        func_80047230(rot, (f32)cam->unk_100, zero, 1.0f, zero);
        func_80046B70(rot, trans, m);
        func_80046C4C(m, (f32)(sx * 4), zero, (f32)(-sy * 4),
                      &cam->pos.x, &cam->pos.y, &cam->pos.z);
    }
    if (btn & 0xF00) {
        zero = 0.0f;
        func_80047790(trans, cam->pos.x, zero, cam->pos.z);
        func_80047230(rot, (f32)cam->unk_100, zero, 1.0f, zero);
        func_80046B70(rot, trans, m);
        func_80046C4C(m,
                      (f32)((s32)((btn >> 8) & 1) - (s32)((btn >> 9) & 1)) * 50.0f * 4.0f,
                      zero,
                      (f32)((s32)((btn >> 10) & 1) - (s32)((btn >> 11) & 1)) * 50.0f * 4.0f,
                      &cam->pos.x, &cam->pos.y, &cam->pos.z);
    }

    if (cam->pos.x < 0.0f) {
        cam->pos.x = 0.0f;
    } else {
        lim = 600.0f;
        lim = cam->pos.x / lim;
        if ((f32)func_8000DD60() < lim) {
            cam->pos.x = func_8000DD60() * 600 - 1;
        }
    }
    if (cam->pos.z < 0.0f) {
        cam->pos.z = 0.0f;
    } else {
        lim = 600.0f;
        lim = cam->pos.z / lim;
        if ((f32)func_8000DD74() < lim) {
            cam->pos.z = func_8000DD74() * 600 - 1;
        }
    }

    if (btn & 0xC) {
        if (btn & 4) {
            cam->unk_FC += 2;
            if (cam->unk_FC > 0) {
                cam->unk_FC = 0;
            }
        } else {
            cam->unk_FC -= 2;
            if (cam->unk_FC < -89) {
                cam->unk_FC = -89;
            }
        }
    }
    if (btn & 3) {
        if (btn & 1) {
            cam->unk_100 = (cam->unk_100 + 6) % 360;
        } else {
            cam->unk_100 = (cam->unk_100 - 6) % 360;
        }
    }
    if (btn & 0x30) {
        if (btn & 0x20) {
            cam->unk_104 += 120;
        } else {
            cam->unk_104 -= 120;
        }
        if (cam->unk_104 > 10000) {
            cam->unk_104 = 10000;
        }
        if (cam->unk_104 < 700) {
            cam->unk_104 = 700;
        }
    }
    cam->unk_5C = 0.0f;
    cam->unk_58 = cam->pos.x;
    cam->unk_60 = cam->pos.z;
    func_8000FB08(cam, cam->pos);
}
