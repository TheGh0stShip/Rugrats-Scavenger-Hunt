/* func_8002D0A8  VRAM 0x8002D0A8  ROM 0x2DCA8  Size: 0x334
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Obj_8005B214 {
    u8 pad_000[0x1A8];
    u8 f1A8;            /* 0x1A8 */
};

struct Obj_800529F0 {
    u8 pad_000[0x11C];
    s32 f11C;           /* 0x11C */
};

struct Seg_80054460 {
    u8 pad_0[3];
    u8 value;           /* 0x3 */
};

extern u32 func_800166B0(struct Obj_8005B214 *);
extern int func_8001F588(s32);
extern s32 func_8002A604(void);
extern void func_800137E8(s32);
extern void func_80033118(void);
extern void func_8001380C(s32, const char *, s32, s32);
extern s32 func_80047C20(void);
extern void func_8002A5B0(s32);
extern void func_8002B530(s32);
extern void func_8002A5EC(s32);
extern void func_80005500(s32, struct Obj_8005B214 *);
extern u8 *func_80019788(s32);
extern void func_8000CE7C(u8 *, s32);
extern void func_8002B2E4(void);
extern int func_8001F138(s32);

extern struct Obj_8005B214 *D_8005B214;
extern struct Obj_800529F0 *D_800529F0;
extern const char D_80002670[];
extern s32 D_800537D4;
extern s32 D_800537D8;
extern s32 D_800537DC;
extern s32 D_800537E0;
extern u8 D_800537B8[];
extern s32 D_800537C0[];
extern u8 D_800544D0;
extern s32 D_800535E4;
extern s32 D_80054550;
extern s32 D_8005455C;
extern struct Seg_80054460 D_80054460[];

void func_8002D0A8(void) {
    u32 buttons;
    s32 st;
    s32 a;

    buttons = func_800166B0(D_8005B214);
    func_8001F588(0);
    if (func_8002A604() != 14) {
        st = D_800537DC;
        if (st < 0) {
            func_800137E8(8);
            func_80033118();
            if (D_800529F0->f11C >= 0) {
                func_8001380C(0x37, D_80002670, 0xB0, 0xB4);
            }
            if (buttons & 0x10) {
                s32 r;
                D_800537DC = 4;
                D_800537D8 = D_800544D0;
                r = func_80047C20();
                D_800535E4 = 0;
                D_800537E0 = r % 10 + 1;
            } else if (buttons & 0x40) {
                func_8002A5B0(4);
                func_8002B530(9);
                D_800537D8 = 0;
                D_800537D4 = 0;
            }
        } else if (--D_800537E0 == 0) {
            D_800537DC = --st;
            if (st >= 0) {
                D_800537D8 = D_800537B8[st];
                D_800537E0 = D_800537C0[st];
            } else {
                a = (D_800537D4 + 30) / 60 * 60;
                a %= 360;
                D_800537D8 = 0;
                D_800537D4 = a;
                D_8005B214->f1A8 = D_80054460[a / 60].value;
                func_8002A5EC(14);
                func_80005500(11, D_8005B214);
            }
        }
    }
    func_8000CE7C(func_80019788(D_80054550) + 0x2C, D_800537D4 / 10);
    func_8002B2E4();
    if (D_800537D4 / 30 != D_8005455C / 30) {
        D_8005455C = D_800537D4;
        func_8001F138(0x15);
    }
    D_800537D4 = (D_800537D4 + D_800537D8) % 360;
}
