/* Target: func_8002E544  VRAM 0x8002E544  ROM 0x2F144
 * Size: 0x74  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: callee takes a second arg elsewhere; called unprototyped here */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Obj_8002E0E0 {
    u8 pad_000[0x18C];
    u32 flags;          /* 0x18C */
    u8 pad_190[0x4];
    s16 timer;          /* 0x194 */
    u8 pad_196[0x1E8 - 0x196];
    s16 f1E8[4];        /* 0x1E8 */
    s16 f1F0[4];        /* 0x1F0 */
    s16 f1F8;           /* 0x1F8 */
};

extern s32 func_800301C8(void *, int);

void func_8002E544(struct Obj_8002E0E0 *o, s32 arg1) {
    if (o->timer > 0) {
        o->timer--;
        return;
    }
    o->timer = 40;
    if (o->f1F8 > 0 && func_800301C8(o, arg1) == 0) {
        o->flags |= 1;
    } else {
        o->flags |= 0x10;
    }
}
