/* Target: func_8001ABC8  VRAM 0x8001ABC8  ROM 0x1B7C8
 * Size: 0x200  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

struct Cam_80052F04 {
    u8 pad_00[0xF0];
    s32 unkF0;   /* 0xF0 */
};

struct Slot_210 {
    u8 pad_000[0x1C];
    s16 unk1C;   /* 0x1C */
    u8 pad_01E[0x1F2];
};

struct World_800581F0 {
    u8 pad_0000[0x2000];
    struct Slot_210 slots[1]; /* 0x2000 */
};

extern struct Cam_80052F04 *D_80052F04;
extern struct World_800581F0 *D_800581F0;
extern s8 D_800581E9;
extern s32 func_8001BF3C(void);
extern s32 func_8001BF00(void);
extern void func_8001BF24(s32);
extern void func_8001BED8(s32);
extern s32 func_80012CE8(void);
extern void func_8000B8A8(s32);
extern void func_8000B904(void);
extern void func_80012D9C(s32);
extern void func_80012D7C(s32);
extern void func_8000E630(struct Cam_80052F04 *);
extern void func_80010AE8(struct Cam_80052F04 *);
extern void func_80019BD4(s32);
extern void *func_800197C8(s32);
extern void func_80033E90(void);
extern s32 func_8001C36C(void);
extern s32 func_8001C340(void);

void func_8001ABC8(void) {
    s32 st;

    switch (func_8001BF3C()) {
        case 0x1F:
            if (func_80012CE8() == 1) {
                func_8001BF24(0x20);
                func_8000B8A8(0x20000000);
                func_80012D9C(10);
                func_8000E630(D_80052F04);
                D_80052F04->unkF0 = 0x13;
                func_80019BD4(D_800581F0->slots[D_800581E9].unk1C);
                func_800197C8(0x54);
            }
            break;
        case 0x20:
            if (func_80012CE8() == 1) {
                func_8001BF24(0);
            }
            break;
        case 0x21:
            if (func_80012CE8() == 1) {
                func_8001BF24(0x22);
                func_8000B904();
                func_80012D9C(10);
                func_80010AE8(D_80052F04);
            }
            break;
        case 0x22:
            if (func_80012CE8() == 1) {
                func_8001BED8(4);
                func_8001BF24(0);
            }
            break;
    }
    st = func_8001BF3C();
    if (st != 0x1F && st != 0x22 && func_8001BF00() == 9) {
        func_80033E90();
    }
    if (func_8001BF00() == 9 && func_8001BF3C() == 0 &&
        (func_8001C36C() != 0 || func_8001C340() != 0)) {
        func_8001BF24(0x21);
        func_80012D7C(10);
    }
}
