typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Actor_80005EF8 {
    u8 pad_000[0x2C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_038[0xE0 - 0x38];
    s32 unk_E0;
    u8 pad_0E4[0xF4 - 0xE4];
    s32 unk_F4;
    s32 unk_F8;
    u8 pad_0FC[0x118 - 0xFC];
    u32 flags;
    u8 pad_11C[0x13C - 0x11C];
    u8 gridX;
    u8 gridZ;
    u8 pad_13E[0x150 - 0x13E];
    s16 unk_150;
    s16 unk_152;
} Actor_80005EF8;

typedef struct Vec3f {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

typedef struct Cam_80052F04 {
    u8 pad_000[0x58];
    Vec3f pos;
    u8 pad_064[0xF0 - 0x64];
    s32 unk_F0;
    u8 pad_0F4[0xFC - 0xF4];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
} Cam_80052F04;

typedef struct Level_800581F0 {
    u8 pad_0000[0x2014];
    s16 originZ;
    s16 originX;
} Level_800581F0;

extern s32 D_80052F18;
extern Cam_80052F04 *D_80052F04;
extern s32 D_8005300C[];
extern s32 D_80057D5C;
extern s8 D_800581E9;
extern u8 *D_800581F0;

extern void *func_800197C8(s32 a);
extern void func_80006240(Actor_80005EF8 *p);
extern void func_80012D04(s32 a, s32 b);
extern void func_800065FC(Actor_80005EF8 *p);
extern void func_8000DDAC(u8 a);
extern s32 func_8000E108(void);
extern void func_8001BED8(s32 a);
extern void func_80019BD4(s32 a);
extern void func_80026310(s32 a);
extern void func_80027FD4(void);
extern s32 func_80005598(Actor_80005EF8 *p);
extern void func_80005500(s32 a, Actor_80005EF8 *p);
extern void func_80024534(Actor_80005EF8 *p);
extern void func_800069C8(s32 a, Actor_80005EF8 *p, s32 b);
extern void func_8000FB08(Cam_80052F04 *c, Vec3f pos);
extern s32 func_8001EEAC(void);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32 a, s32 b);
extern void func_8001E678(s32 a, s32 b);
extern s32 func_8001BF00(void);
extern void func_8001DEE8(void);
extern void func_8001E7DC(s32 a);

void func_80005EF8(Actor_80005EF8 *p) {
    s32 flag;
    s32 i;
    s32 v;
    Level_800581F0 *lvl;

    flag = 0;
    func_800197C8(p->unk_E0);
    if (D_80052F18 == 2) {
        func_80006240(p);
    } else {
        func_80012D04(10, 0);
        func_800065FC(p);
        func_8000DDAC(*((u8 *)p + 0x151));
        lvl = (Level_800581F0 *)(D_800581F0 + D_800581E9 * 0x210);
        v = p->unk_152;
        p->x = (p->gridX - lvl->originX) * 600 + 300;
        p->z = (p->gridZ - lvl->originZ) * 600 + 300;
        p->y = 0;
        if (v == func_8000E108()) {
            v = p->unk_150;
            if (v != func_8000E108()) {
                flag = 1;
                func_8001BED8(3);
                func_80019BD4(D_8005300C[p->unk_F8]);
                for (i = 0; i < 0x211; i++) {
                    func_80019BD4(i);
                }
                if (D_80052F18 == 0) {
                    func_80026310(0);
                } else {
                    func_80026310(1);
                }
                func_80027FD4();
                p->flags &= ~0x10000;
            }
        }
        if (p->flags & 0x80) {
            if (func_80005598(p) == 0) {
                func_80005500(0, p);
            } else {
                func_80005500(5, p);
                if (p->flags & 4) {
                    func_80024534(p);
                }
            }
        } else if (flag) {
            func_80005500(0, p);
        } else {
            func_80005500(6, p);
        }
        func_800069C8(0, p, 1);
        v = p->unk_150;
        if (v == func_8000E108()) {
            D_80052F04->pos.x = 1500.0f;
            D_80052F04->pos.y = 0;
            D_80052F04->unk_FC = -10;
            D_80052F04->pos.y = 500.0f;
            D_80052F04->unk_100 = 0x10E;
            D_80052F04->pos.z = 900.0f;
            D_80052F04->unk_104 = 0x578;
            func_8000FB08(D_80052F04, D_80052F04->pos);
            D_80052F04->unk_F0 = 12;
            p->flags &= ~0x200000;
            if (func_8001EEAC() != 0x209) {
                func_8001EEBC();
                func_8001ECE4(0x209, 0x38);
                func_8001E678(10, 0);
                for (i = 0; i < 0x211; i++) {
                    func_80019BD4(i);
                }
                if (D_80052F18 == 0) {
                    func_80026310(0);
                } else {
                    func_80026310(1);
                }
                func_80027FD4();
            }
        }
    }
    if (func_8001BF00() != 0x30) {
        func_8001DEE8();
    }
    func_8001E7DC(10);
    if (p->unk_F4 == 0 || p->unk_F4 == D_80057D5C) {
        func_8001E678(10, 0);
    }
}
