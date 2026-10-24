/* func_8002FAF8: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Pair_8002FAF8 {
    u8 *a;
    u8 *b;
};
struct Player_8002FAF8 {
    u8 pad_00[0xF7];
    u8 unkF7;
    u8 pad_F8[0xF0];
    s16 unk1E8[4];
    s16 unk1F0[4];
    s16 unk1F8;
};
struct Info_8002FAF8 {
    u8 pad_00[0x3];
    u8 unk3;
};
extern s32 D_800B9314;
extern s32 D_800B9308;
extern s32 D_80057D40;
extern struct Pair_8002FAF8 D_80054570[];
extern struct Pair_8002FAF8 D_80054590[];
extern struct Player_8002FAF8 *func_800055A4(s32);
extern u32 func_800166B0(struct Player_8002FAF8 *);
extern s32 func_8002FED0(void);
extern s32 func_8002FEEC(void);
extern struct Info_8002FAF8 *func_8002A628(s32);
extern void func_8002FEC4(s32);
extern void func_8002FEE0(s32);
extern int func_8001F138(s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, int, int);

void func_8002FAF8(void) {
    struct Player_8002FAF8 *pl;
    u32 btn;
    u32 idx;
    u8 *base;
    u8 *col;
    s32 t;

    if (--D_800B9314 <= 0) {
        pl = func_800055A4(D_80057D40);
        btn = func_800166B0(pl);
        func_8002FED0();
        idx = func_8002A628(1)->unk3;
        func_8002FEE0(idx);
        func_8002FEC4(pl->unkF7);
        if (btn & 2) {
            if ((*(idx + pl->unk1E8)) > D_800B9308) {
                (*(idx + pl->unk1E8))--;
                pl->unk1F8++;
                func_8001F138(0x1C);
                D_800B9314 = 5;
            }
        } else if (btn & 1) {
            if ((*(idx + pl->unk1E8)) < (*(idx + pl->unk1F0)) && pl->unk1F8 > 0) {
                (*(idx + pl->unk1E8))++;
                pl->unk1F8--;
                func_8001F138(0x1C);
                D_800B9314 = 5;
            }
        }
    }

    base = D_80054570[(u8)func_8002FED0()].a;
    col = D_80054570[(u8)func_8002FED0()].b;
    func_80033144(0, 0, 0xFF, 0x7F);
    func_80032A20(func_80032870(0x2A), base, col);
    func_80032A20(func_80032870(0x2A), base + 0x50, col);
    func_80032A20(func_80032870(0x2A), base + 0xA0, col);
    func_80032A20(func_80032870(0x2A), base + 0xF0, col);
    func_80032A20(func_80032870(0x2B), base + 0x140, col);
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0x2C), base, col);
    func_80032A20(func_80032870(0x2D), base + 0x50, col);
    func_80032A20(func_80032870(0x2D), base + 0xA0, col);
    func_80032A20(func_80032870(0x2D), base + 0xF0, col);
    func_80032A20(func_80032870(0x2E), base + 0x140, col);

    base = D_80054590[(u8)func_8002FEEC()].a;
    t = func_8002FEEC();
    col = D_80054590[(u8)t].b + (u8)func_8002FED0() * 24;
    func_80033144(0, 0xC8, 0, 0x7F);
    func_80032A20(func_80032870(0x2F), base, col);
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0x30), base, col);

    base = D_80054590[4].a;
    col = D_80054590[4].b + (u8)func_8002FED0() * 24;
    func_80033144(0, 0xC8, 0, 0x7F);
    func_80032A20(func_80032870(0x2F), base, col);
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    func_80032A20(func_80032870(0x30), base, col);
}
