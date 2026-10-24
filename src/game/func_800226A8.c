/* Target: func_800226A8  VRAM 0x800226A8  ROM 0x232A8
 * Size: 0xB8  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

struct Obj_800226A8 {
    u8 pad_000[0x144];
    s32 unk144;
};

extern u16 D_80052AE8[];
extern s32 func_80024660(void *);
extern struct Obj_800226A8 *func_800206D0(void);
extern void func_80025498(struct Obj_800226A8 *, s16);

s32 func_800226A8(void) {
    s32 done = 0;
    s32 ret = 0;
    struct Obj_800226A8 *p;
    s16 v;

    if (func_80024660(0)) {
        done = 1;
        ret = 3;
    }
    if (!done) {
        p = func_800206D0();
        v = D_80052AE8[p->unk144];
        switch (v) {
            case 6:
            case 27:
            case 31:
            case 35:
            case 39:
                func_80025498(p, v);
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
            case 26:
            case 28:
            case 29:
            case 30:
            case 32:
            case 33:
            case 34:
            case 36:
            case 37:
            case 38:
                ret = 2;
                break;
        }
    }
    return ret;
}
