/* Target: func_8002E32C  VRAM 0x8002E32C  ROM 0x2EF2C
 * Size: 0x14C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
 * Unresolved: func_8002A628 return type (pointer to s32) guessed */

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
extern s32 *func_8002A628(s32);

void func_8002E32C(struct Obj_8002E0E0 *o) {
    if (o->timer > 0) {
        o->timer--;
        return;
    }
    o->timer = 40;
    switch (*func_8002A628(0)) {
        case 1:
            if (o->f1F8 >= 5 && func_800301C8(o, 0) == 0) {
                o->flags |= 0x10;
                return;
            }
            break;
        case 2:
            if (o->f1F8 >= 5 && func_800301C8(o, 1) == 0) {
                o->flags |= 0x10;
                return;
            }
            break;
        case 3:
            if (o->f1F8 >= 5 && func_800301C8(o, 2) == 0) {
                o->flags |= 0x10;
                return;
            }
            break;
        case 4:
            if (o->f1F8 >= 5 && func_800301C8(o, 3) == 0) {
                o->flags |= 0x10;
                return;
            }
            break;
        default:
            return;
    }
    o->flags |= 0x40;
}
