/* func_8000715C: SN GCC 2.8 -O2 -G0, .rdata jump table at 0x800005B0,
 * reviewed scheduling normalization (normalize_d1_func_8000715C).
 * The empty asm statements hide the unit step constants from post-reload
 * CSE so the switch arms keep their immediate adds. */
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Actor_8000715C {
    /* 0x000 */ u8 pad_000[0x2C];
    /* 0x02C */ f32 x;
    /* 0x030 */ f32 y;
    /* 0x034 */ f32 z;
    /* 0x038 */ u8 pad_038[0xE0 - 0x38];
    /* 0x0E0 */ s32 unk_E0;
    /* 0x0E4 */ u8 pad_0E4[0xF8 - 0xE4];
    /* 0x0F8 */ s32 unk_F8;
    /* 0x0FC */ s32 state;
    /* 0x100 */ u8 pad_100[0x118 - 0x100];
    /* 0x118 */ u32 flags;
    /* 0x11C */ u8 pad_11C[0x128 - 0x11C];
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ s32 unk_12C;
    /* 0x130 */ u8 pad_130[0x13C - 0x130];
    /* 0x13C */ u8 gridX;
    /* 0x13D */ u8 gridZ;
    /* 0x13E */ u8 pad_13E[0x1A6 - 0x13E];
    /* 0x1A6 */ s8 unk_1A6;
} Actor_8000715C;

typedef struct Anim_8000715C {
    /* 0x0 */ u8 pad_0[8];
    /* 0x8 */ s16 state;
    /* 0xA */ s16 unk_A;
} Anim_8000715C;

typedef struct Cam_80052F04 {
    /* 0x000 */ u8 pad_000[0xF0];
    /* 0x0F0 */ s32 unk_F0;
} Cam_80052F04;

extern s32 D_80052FFC[];
extern s32 D_8005300C[];
extern s32 D_800529F8;
extern s32 D_800535E4;
extern s16 D_80057DA0;
extern u8 D_80057D68[][2];
extern Cam_80052F04 *D_80052F04;

extern u8 *func_800197C8(s32 a);
extern s32 func_80006F8C(Actor_8000715C *p, u8 x, u8 z, s32 *outX, s32 *outZ);
extern f32 func_80007A28(Actor_8000715C *p);
extern s32 func_80005598(Actor_8000715C *p);
extern s16 func_8000DF14(s16 z, s16 x);
extern s16 func_8000DF48(s16 tile);
extern s16 func_8000DF94(s16 tile);
extern void func_80005500(s32 a, Actor_8000715C *p);
extern void func_8000798C(Actor_8000715C *p, s32 dx, s32 dz);
extern void func_800069C8(s32 anim, Actor_8000715C *p, s32 loop);
extern void func_8001D418(Actor_8000715C *p, s32 n);

void func_8000715C(Actor_8000715C *p) {
    Anim_8000715C *anim;
    s32 idle = 0;
    s32 dx;
    s32 dz;
    s32 nx;
    s32 nz;
    s32 tx;
    s32 tz;
    f32 speed;
    u8 *e;
    s16 v;

    if (p->flags & 0x20) {
        anim = (Anim_8000715C *)(func_800197C8(D_80052FFC[p->unk_F8]) + 0x2C);
    } else if (p->flags & 0x10000) {
        anim = (Anim_8000715C *)(func_800197C8(D_8005300C[p->unk_F8]) + 0x2C);
    } else {
        anim = (Anim_8000715C *)(func_800197C8(p->unk_E0) + 0x2C);
    }

    if (p->state == 1) {
        switch (p->unk_128 / 90) {
            case 0:
            case 4:
                nx = p->gridX;
                nz = p->gridZ + 1;
                dx = 0;
                __asm__("" : "=r"(dz) : "0"(1));
                break;
            case 1:
                nz = p->gridZ;
                nx = p->gridX + 1;
                __asm__("" : "=r"(dx) : "0"(1));
                dz = 0;
                break;
            case 2:
                nx = p->gridX;
                nz = p->gridZ - 1;
                dx = 0;
                __asm__("" : "=r"(dz) : "0"(-1));
                break;
            case 3:
                nz = p->gridZ;
                nx = p->gridX - 1;
                __asm__("" : "=r"(dx) : "0"(-1));
                dz = 0;
                break;
        }
        if (func_80006F8C(p, nx, nz, &tx, &tz) != 0) {
            speed = func_80007A28(p);
            p->x += dx * speed;
            p->z += dz * speed;
            if (dx < 0 && p->x < tx) {
                p->x = tx;
            }
            if (dx > 0 && tx < p->x) {
                p->x = tx;
            }
            if (dx == 0) {
                p->x = tx;
            }
            if (dz < 0 && p->z < tz) {
                p->z = tz;
            }
            if (dz > 0 && tz < p->z) {
                p->z = tz;
            }
            if (dz == 0) {
                p->z = tz;
            }
            if (p->z == (f32)tz && p->x == (f32)tx) {
                if (func_80005598(p) != 0) {
                    if (D_800535E4 == 0) {
if (D_80057DA0 != 0) { s32 o = (D_80057DA0 - 1) << 1; e = (u8 *)D_80057D68 + o;
                            if ((u8)nx == e[0] && (u8)nz == e[1]) {
                                if (func_8000DF48(func_8000DF14(p->gridZ, p->gridX)) == 0) {
                                    D_800529F8++;
                                    if (!(p->flags & 0x20) && p->unk_1A6 != 0) {
                                        p->unk_1A6++;
                                    }
                                }
                                D_80057DA0--;
                                goto arrive;
                            }
                        }
                        D_80057D68[D_80057DA0][0] = p->gridX;
                        D_80057D68[D_80057DA0][1] = p->gridZ;
                        D_80057DA0++;
                        if (func_8000DF48(func_8000DF14((u8)nz, (u8)nx)) == 0) {
                            D_800529F8--;
                            if (p->unk_1A6 > 0 && !(p->flags & 0x20)) {
                                p->unk_1A6--;
                            }
                        }
                    }
                } else if (func_8000DF94(func_8000DF14((u8)nz, (u8)nx)) == 0) {
                    D_800529F8--;
                }
            arrive:
                p->unk_12C = p->unk_128;
                func_80005500(6, p);
                func_8000798C(p, dx, dz);
            } else {
                v = anim->state;
                if (v == 5 || v == 13) {
                    func_800069C8(1, p, 2);
                } else if (v != 0 && v != 8) {
                    func_800069C8(1, p, 2);
                } else {
                    func_800069C8(5, p, 0);
                    if (anim->unk_A == 0) {
                        func_800069C8(1, p, 0);
                    }
                }
            }
        } else {
            func_80005500(6, p);
            idle = 1;
        }
    } else if ((u32)(p->state - 2) >= 2) {
        if (anim->state != 2 && !(p->flags & 0x800000)) {
            idle = 1;
        }
    }

    if (idle) {
        v = anim->state;
        if (v == 6 || v == 14 || (v != 1 && v != 9)) {
            func_800069C8(0, p, 2);
        } else {
            func_800069C8(6, p, 2);
            if (anim->unk_A == 0) {
                func_800069C8(0, p, 0);
            }
        }
    }
    if (D_80052F04->unk_F0 == 0 && p->state != 10) {
        func_8001D418(p, D_800529F8);
    }
}
