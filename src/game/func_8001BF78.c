/* Target: func_8001BF78  VRAM 0x8001BF78  ROM 0x1CB78
 * Size: 0x334  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;

struct Player_800055A4 {
    u8 pad_00[0x11C];
    s32 unk11C;    /* 0x11C */
    u8 pad_120[0x88];
    u8 unk1A8;     /* 0x1A8 */
};

struct Rec_800197C8 {
    u8 pad_00[0x2C];
    u8 obj2C[1];   /* 0x2C */
};

struct Prize_80053604 {
    u8 pad_00[3];
    u8 value;      /* 0x03 */
};

extern struct Player_800055A4 *D_8005B214;
extern struct Player_800055A4 *D_800529F0;
extern s32 D_800537DC;
extern s32 D_800537D8;
extern s32 D_800537E0;
extern s32 D_800537D4;
extern s32 D_800535E4;
extern s32 D_80053858;
extern s32 D_800536FC;
extern u8 D_800537B8[];
extern u8 D_800537BC;
extern s32 D_800537C0[];
extern struct Prize_80053604 D_80053604[];
extern char D_800018E0[];
extern u32 func_800166B0(struct Player_800055A4 *);
extern int func_8001F588(s32);
extern s32 func_8001BF3C(void);
extern void func_800137E8(s32);
extern void func_80033118(void);
extern void func_8001380C(s32, char *, s32, s32);
extern s32 func_80047C20(void);
extern void func_8001BED8(s32);
extern void func_8001A8BC(s32);
extern void func_8001BF24(s32);
extern void func_80005500(s32, void *);
extern struct Rec_800197C8 *func_800197C8(s32);
extern void func_8000CE7C(void *, s32);
extern void func_8001A754(void);
extern int func_8001F138(s32);

void func_8001BF78(void) {
    u32 buttons = func_800166B0(D_8005B214);
    s32 a;

    func_8001F588(0);
    if (func_8001BF3C() != 0xE) {
        if (D_800537DC < 0) {
            func_800137E8(8);
            func_80033118();
            if (D_800529F0->unk11C >= 0) {
                func_8001380C(0x37, D_800018E0, 0xB0, 0xB4);
            }
            if (buttons & 0x10) {
                D_800537DC = 4;
                D_800537D8 = D_800537BC;
                D_800535E4 = 0;
                D_800537E0 = func_80047C20() % 10 + 1;
            } else if (buttons & 0x40) {
                func_8001BED8(4);
                func_8001A8BC(9);
                D_800537D8 = 0;
                D_800537D4 = 0;
            }
        } else if (--D_800537E0 == 0) {
            if (--D_800537DC >= 0) {
                D_800537D8 = D_800537B8[D_800537DC];
                D_800537E0 = D_800537C0[D_800537DC];
            } else {
                a = ((D_800537D4 + 39) / 60 * 60) % 360;
                D_800537D8 = 0;
                D_800537D4 = a;
                D_8005B214->unk1A8 = D_80053604[a / 60].value;
                func_8001BF24(0xE);
                func_80005500(0xB, D_8005B214);
            }
        }
    }
    func_8000CE7C(func_800197C8(D_800536FC)->obj2C, D_800537D4 / 10);
    func_8001A754();
    if (D_800537D4 / 30 != D_80053858 / 30) {
        D_80053858 = D_800537D4;
        func_8001F138(0x15);
    }
    D_800537D4 = (D_800537D4 + D_800537D8) % 360;
}
