/* func_8002F784  VRAM 0x8002F784  ROM 0x30384  Size: 0x374
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Pair_8002F784 {
    u8 *a;
    u8 *b;
};
struct Player_8002F784 {
    u8 pad_00[0xF7];
    u8 unkF7;
};
extern s32 D_800B9314;
extern s32 D_80057D40;
extern s32 D_80057D5C;
extern struct Pair_8002F784 D_80054570[];
extern struct Pair_8002F784 D_80054590[];
extern struct Player_8002F784 *func_800055A4(s32);
extern u32 func_800166B0(struct Player_8002F784 *);
extern s32 func_8002FED0(void);
extern s32 func_8002FEEC(void);
extern s32 func_8002FEB4(void);
extern void func_8002FEC4(s32);
extern void func_8002FEE0(s32);
extern int func_8001F138(s32);
extern void func_80033144(s32, s32, s32, s32);
extern void *func_80032870(s32);
extern void func_80032A20(void *, int, int);

void func_8002F784(void) {
    struct Player_8002F784 *pl;
    u32 btn;
    s32 sel;
    s32 sub;
    u8 *base;
    u8 *col;
    s32 t;

    if (--D_800B9314 <= 0) {
        pl = func_800055A4(D_80057D40);
        btn = func_800166B0(pl);
        sel = func_8002FED0();
        sub = func_8002FEEC();
        if (func_8002FEB4() != 0x10) {
            if (btn & 8) {
                if ((u8)sel < D_80057D5C - 1) {
                    func_8002FEC4((u8)(sel + 1));
                    func_8001F138(0x1C);
                    D_800B9314 = 5;
                }
            } else if ((btn & 4) && (u8)sel != 0) {
                func_8002FEC4((u8)(sel - 1));
                func_8001F138(0x1C);
                D_800B9314 = 5;
            }
        } else {
            func_8002FEC4(pl->unkF7);
        }
        if (btn & 2) {
            if ((u8)sub < 4 && (func_8002FEB4() != 0x10 || (u8)sub != 3)) {
                func_8002FEE0((u8)(sub + 1));
                func_8001F138(0x1C);
                D_800B9314 = 5;
            }
        } else if ((btn & 1) && (u8)sub != 0) {
            func_8002FEE0((u8)(sub - 1));
            func_8001F138(0x1C);
            D_800B9314 = 5;
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
}
