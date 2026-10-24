/* Target: func_8002BFF0  VRAM 0x8002BFF0  ROM 0x2CBF0
 * Size: 0x178  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef short s16;
typedef int s32;
typedef unsigned char u8;

struct Actor_8002BFF0 {
    u8 pad_000[0x11C];
    s32 unk11C;           /* 0x11C */
};

extern s32 D_800B9298;
extern void *D_80052F04;
extern s32 func_8002A604(void);
extern void func_8002A5EC(s32 value);
extern void func_8002E32C(struct Actor_8002BFF0 *a);
extern s32 func_800166B0(struct Actor_8002BFF0 *a);
extern s32 *func_8002A628(s32 a);
extern s32 func_8002987C(struct Actor_8002BFF0 *a, s32 b, s32 c);
extern void func_8000E630(void *p);
extern void func_80024D14(struct Actor_8002BFF0 *a, s16 b, s32 c);
extern void func_8002A5B0(s32 a);
extern void func_8002E6D0(struct Actor_8002BFF0 *a, s32 b, s32 c);
extern void func_800255AC(struct Actor_8002BFF0 *a);
extern void func_8001A5C0(void);

void func_8002BFF0(struct Actor_8002BFF0 *arg0) {
    s32 idx;
    s32 anim;
    s32 info;
    s32 r;

    if (func_8002A604() == 1) {
        D_800B9298 = 0x23;
        func_8002A5EC(3);
    }
    if (arg0->unk11C == -1) {
        func_8002E32C(arg0);
    }
    info = func_800166B0(arg0);
    switch (*func_8002A628(0)) {
        case 1:
            anim = 0x1B;
            idx = 0;
            break;
        case 2:
            anim = 0x1C;
            idx = 1;
            break;
        case 3:
            anim = 0x1A;
            idx = 2;
            break;
        case 4:
            anim = 0x19;
            idx = 3;
            break;
    }
    r = func_8002987C(arg0, idx, info);
    if (r == 2) {
        func_8000E630(D_80052F04);
        func_80024D14(arg0, anim, 0x37);
        func_8002A5B0(0);
        func_8002E6D0(arg0, 8, -5);
    } else if (r == 1) {
        func_800255AC(arg0);
        func_8001A5C0();
    }
}
