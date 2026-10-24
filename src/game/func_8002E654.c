/* Target: func_8002E654  VRAM 0x8002E654  ROM 0x2F254
 * Size: 0x74  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: none significant */

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

extern u8 func_8002FEEC(void);
extern u32 D_800B92F4;

void func_8002E654(struct Obj_8002E0E0 *o) {
    if (o->timer > 0) {
        o->timer--;
        return;
    }
    o->timer = 40;
    if (func_8002FEEC() != D_800B92F4) {
        o->flags |= 2;
    } else {
        o->flags |= 0x10;
    }
}
