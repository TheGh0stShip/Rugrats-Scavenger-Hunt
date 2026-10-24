/* func_8000E7C8  ROM 0xF3C8  Size: 0x988  (camera state machine)
 * Best SN GCC 2.8 -O2 attempt; not byte-exact. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Vec3f {
    f32 x, y, z;
} Vec3f;

typedef struct Actor {
    u8 pad_00[0x2C];
    Vec3f pos;          /* 0x2C */
    u8 pad_38[0xF8 - 0x38];
    s32 unkF8;
    u8 pad_FC[0x11C - 0xFC];
    s32 unk11C;
    u8 pad_120[0x4];
    s32 unk124;
    u8 pad_128[0x1FC - 0x128];
    s32 unk1FC;
} Actor;

typedef struct Cam {
    u8 pad_00[0x28];
    Vec3f at;           /* 0x28 */
    u8 pad_34[0x58 - 0x34];
    Vec3f pos;          /* 0x58 */
    u8 pad_64[0xEC - 0x64];
    u32 steps;          /* 0xEC */
    s32 state;          /* 0xF0 */
    u8 pad_F4[0x4];
    Actor *target;      /* 0xF8 */
    s32 unkFC;
    s32 unk100;
    s32 unk104;
} Cam;

typedef struct Trans_800588F8 {
    f32 d[6];           /* 0x00 per-step deltas */
    f32 cur[6];         /* 0x18 current (3 ints as float, then pos) */
    s32 dst[3];         /* 0x30 */
    Vec3f dstPos;       /* 0x3C */
} Trans_800588F8;

typedef struct Ctl_800C33D8 {
    Vec3f a;            /* 0x00 */
    Vec3f b;            /* 0x0C */
    s32 unk18;
    s32 unk1C;
    s32 dist;           /* 0x20 */
    u8 pad_24[0x4];
    s32 count;          /* 0x28 */
    s32 unk2C;
    f32 dx;             /* 0x30 */
    u8 pad_34[0x4];
    f32 dz;             /* 0x38 */
} Ctl_800C33D8;

typedef struct Info_80058B58 {
    u8 pad_00[0x4];
    u16 flags;
    u8 pad_06[0x4];
} Info_80058B58; /* size 10 */

typedef struct Mode_80052F18 {
    s32 mode;
    u8 pad_04[0x18];
    s32 list[1];        /* 0x1C */
} Mode_80052F18;

extern s32 D_8005435C;
extern s32 D_80052EE4;
extern Mode_80052F18 D_80052F18;
extern s32 D_80057D40;
extern s32 D_80052BE8;
extern Info_80058B58 D_80058B58[];
extern Trans_800588F8 D_800588F8;
extern Ctl_800C33D8 D_800C33D8;
extern s32 D_800C33F4;
extern s32 D_800547C0;
extern s32 D_800547C4;
extern s32 D_800547C8;
extern s32 D_800587F0;
extern s32 D_80058670;
extern s32 D_80058674;

extern Actor *func_80005544(void);
extern s32 func_80005598(Actor *a);
extern void func_80005500(s32 a, Actor *b);
extern void func_8000550C(s32 a, Actor *b);
extern void func_8001BED8(s32 a);
extern void func_8000F604(Cam *cam, Actor *a);
extern void func_8000FB08(Cam *cam, Vec3f pos);
extern void func_8000FD38(Cam *cam);
extern void func_8000FD9C(Cam *cam);
extern s32 func_80008B80(Vec3f a, Vec3f b);
extern void func_8000F34C(Cam *cam);
extern void func_8000F4E4(Cam *cam);
extern void func_80010760(Cam *cam);
extern void func_80010358(Cam *cam);
extern void func_800109EC(Cam *cam, s32 idx);
extern void func_80010850(Cam *cam);
extern void func_8000F800(Cam *cam);

void func_8000E7C8(Cam *cam) {
    Actor *pl;
    Trans_800588F8 *t;
    Ctl_800C33D8 *c;
    Vec3f v;
    Vec3f v2;
    s32 n;
    s32 r;
    s32 dx;
    s32 dz;
    f32 f;

    if (D_8005435C == 0) {
        goto end;
    }
    pl = func_80005544();
    if (D_800C33D8.unk2C != 0) {
        D_800C33D8.unk2C = 0;
        cam->state = 14;
    }
    switch (cam->state) {
        case 0:
            func_8000F604(cam, pl);
            v.x = pl->pos.x;
            v.y = cam->pos.y;
            v.z = pl->pos.z;
            func_8000FB08(cam, cam->pos);
            if (D_80052EE4 != 0) {
                cam->state = 13;
            }
            break;

        case 1:
            if (func_80005598(pl) == 0) {
                cam->steps = 1;
            } else if (pl->unk11C == -1) {
                cam->steps = 12;
            } else {
                cam->steps = 18;
            }
            t = &D_800588F8;
            t->cur[0] = cam->unkFC;
            t->cur[1] = cam->unk100;
            t->cur[2] = cam->unk104;
            t->cur[3] = cam->pos.x;
            t->cur[4] = cam->pos.y;
            t->cur[5] = cam->pos.z;
            if (D_80052F18.mode == 2) {
                t->dst[0] = -24;
                t->dst[1] = t->cur[1] + 110.0f;
            } else if (pl->unk1FC == 0) {
                t->dst[0] = -18;
                t->dst[1] = t->cur[1] + 165.0f;
            } else {
                t->dst[0] = -18;
                t->dst[1] = 345;
            }
            if (pl->unk11C == -1) {
                D_800588F8.dst[1] = pl->unk124;
            }
            if (pl->unk11C == -1) {
                D_800588F8.dst[2] = 2400;
            } else {
                s32 a = D_800588F8.dst[1] % 360;
                s32 d = 180 - a;
                if ((f32)d >= 0.0f) {
                    r = D_80052BE8 * d;
                } else {
                    r = D_80052BE8 * (a - 180);
                }
                D_800588F8.dst[2] = r / 180 + 2400;
            }
            t = &D_800588F8;
            t->dstPos = pl->pos;
            if (D_80052F18.mode == 2) {
                if (func_80005598(pl) != 0) {
                    t->dstPos.y = 400.0f;
                } else {
                    t->dstPos.y = 1900.0f;
                }
            } else if (pl->unkF8 == 5) {
                t->dstPos.y = 450.0f;
            } else {
                t->dstPos.y = 233.0f;
            }
            cam->pos.x = D_800588F8.cur[3];
            cam->pos.y = D_800588F8.cur[4];
            cam->pos.z = D_800588F8.cur[5];
            func_8000FB08(cam, cam->pos);
            D_800588F8.d[0] = (f32)(t->dst[0] - cam->unkFC) / (f32)cam->steps;
            t->d[1] = (f32)(t->dst[1] - cam->unk100) / (f32)cam->steps;
            t->d[2] = (f32)(t->dst[2] - cam->unk104) / (f32)cam->steps;
            t->d[3] = (t->dstPos.x - cam->pos.x) / (f32)cam->steps;
            t->d[4] = (t->dstPos.y - cam->pos.y) / (f32)cam->steps;
            t->d[5] = (t->dstPos.z - cam->pos.z) / (f32)cam->steps;
            cam->state = 2;
            break;

        case 2:
            if (D_80058B58[D_80052F18.list[D_80057D40] - 2].flags & 0x8000) {
                cam->steps = 1;
            }
            if (cam->steps != 0) {
                t = &D_800588F8;
                if (--cam->steps != 0) {
                    t->cur[0] += t->d[0];
                    t->cur[1] += t->d[1];
                    t->cur[2] += t->d[2];
                    t->cur[3] += t->d[3];
                    t->cur[4] += t->d[4];
                    t->cur[5] += t->d[5];
                } else {
                    t->cur[0] = t->dst[0];
                    t->cur[1] = t->dst[1];
                    t->cur[2] = t->dst[2];
                    t->cur[3] = t->dstPos.x;
                    t->cur[4] = t->dstPos.y;
                    t->cur[5] = t->dstPos.z;
                }
                t = &D_800588F8;
                cam->unkFC = t->cur[0];
                cam->unk100 = t->cur[1];
                cam->unk104 = t->cur[2];
                cam->pos.x = t->cur[3];
                cam->pos.y = t->cur[4];
                cam->pos.z = t->cur[5];
                func_8000FB08(cam, cam->pos);
                if (cam->steps != 0) {
                    break;
                }
            }
            cam->state = 0;
            if (func_80005598(pl) != 0) {
                func_80005500(6, pl);
                func_8000550C(3, pl);
                func_8001BED8(0);
            } else {
                func_80005500(5, pl);
            }
            break;

        case 3:
            func_8000FD38(cam);
            break;

        case 4:
            func_8000FD9C(cam);
            if (D_80052EE4 != 0) {
                cam->state = 14;
            }
            break;

        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            func_8000FB08(cam, cam->pos);
            break;

        case 11:
            cam->pos.x = cam->target->pos.x;
            cam->pos.y = cam->target->pos.y + 300.0f;
            cam->pos.z = cam->target->pos.z;
            break;

        case 13:
            D_800C33F4 = cam->unk104;
        case 14:
            c = &D_800C33D8;
            r = func_80008B80(c->b, c->a);
            c->dist = r;
            n = r / 200;
            if (!((f32)n >= 0.0f)) {
                n = -n;
            }
            c->count = n;
            c->unk18 = cam->unkFC;
            c->unk1C = cam->unk104;
            if (n < 5) {
                c->count = 5;
            }
            dx = (D_800547C0 - cam->unkFC) / c->count;
            dz = (D_800547C4 - cam->unk104) / c->count;
            f = c->count;
            c->dx = (c->a.x - cam->pos.x) / f;
            c->dz = (c->a.z - cam->pos.z) / f;
            c->b.x = cam->pos.x;
            c->b.z = cam->pos.z;
            cam->state = 15;
            D_800587F0 = ((f32)D_800547C8 - cam->pos.y) / f;
            D_80058674 = dx;
            D_80058670 = dz;
        case 15:
            c = &D_800C33D8;
            c->dist = func_80008B80(cam->at, c->a);
            if (--c->count <= 0) {
                c->b.y = 0;
                c->b.x = c->a.x;
                c->b.z = c->a.z;
                cam->state = 16;
            } else {
                c->b.x += c->dx;
                c->b.z += c->dz;
            }
            func_8000F34C(cam);
            break;

        case 16:
            if (D_800C33D8.unk2C != 0) {
                D_800C33D8.unk2C = 0;
                cam->state = 14;
            } else {
                func_8000F4E4(cam);
            }
            break;

        case 17:
            func_80010760(cam);
            break;
        case 18:
            func_80010358(cam);
            break;
        case 19:
        case 21:
            func_800109EC(cam, D_80057D40);
            break;
        case 20:
        case 22:
            func_80010850(cam);
            break;
    }
end:
    func_8000F800(cam);
}
