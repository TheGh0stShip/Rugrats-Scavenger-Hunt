/* func_8002E1A8  VRAM 0x8002E1A8  ROM 0x2EDA8  Size: 0x184
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
struct Obj_8002E0E0 {
    u8 pad_000[0x11C];
    u32 f11C;           /* 0x11C */
    u8 pad_120[0x18C - 0x120];
    u32 flags;          /* 0x18C */
    u8 pad_190[0x4];
    s16 timer;          /* 0x194 */
    u8 pad_196[0x1E8 - 0x196];
    s16 f1E8[4];        /* 0x1E8 current values */
    s16 f1F0[4];        /* 0x1F0 maximum values */
    s16 f1F8;           /* 0x1F8 */
};

extern s32 func_800301C8(struct Obj_8002E0E0 *, s32);
extern s32 D_800B92F4;
extern s32 D_80054568;

void func_8002E1A8(struct Obj_8002E0E0 *o, s32 mode, s32 n) {
    s32 best = 0;
    s32 idx = 0;
    s32 i;
    s32 d;

    if (o->timer > 0) {
        o->timer--;
        return;
    }
    o->timer = 40;
    switch (mode) {
        case 0:
            if (o->f1F8 >= 5) {
                if (func_800301C8(o, n) == 0) {
                    o->flags |= 0x10;
                } else {
                    o->flags |= 0x40;
                }
            } else {
                o->flags |= 0x40;
            }
            break;
        case 1:
            if (func_800301C8(o, n) == 0) {
                o->flags |= 0x10;
            } else {
                o->flags |= 0x40;
            }
            break;
        case 2:
            if (D_80054568 == 0) {
                for (i = 0; i < 4; i++) {
                    if (func_800301C8(o, i) == 0) {
                        d = -o->f1E8[i] + o->f1F0[i];
                        if (best < d) {
                            best = d;
                            idx = i;
                        }
                    }
                }
                D_800B92F4 = idx;
                D_80054568 = 1;
                o->flags |= 0x10;
            }
            break;
    }
}
