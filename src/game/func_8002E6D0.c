/* func_8002E6D0  VRAM 0x8002E6D0  ROM 0x2F2D0  Size: 0x12C
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
extern int func_8001F138(s32);
extern void func_80010E24(u32, s32);

void func_8002E6D0(struct Obj_8002E0E0 *o, s32 n, s32 delta) {
    if (n != 8 && func_800301C8(o, n) != 0) {
        return;
    }
    if (n == 8) {
        if (o->f1F8 + delta < 0) {
            o->f1F8 = 0;
            goto tail;
        }
        if (o->f1F8 + delta >= 100) {
            o->f1F8 = 99;
            goto tail;
        }
        o->f1F8 += delta;
        if (delta > 0) {
            func_8001F138(4);
        } else if (delta < 0) {
            func_8001F138(0x20);
        }
    } else {
        if (*(o->f1E8 + n) + delta < 0) {
            *(o->f1E8 + n) = 0;
            goto tail;
        }
        if (o->f1F0[n] < *(o->f1E8 + n) + delta) {
            *(o->f1E8 + n) = o->f1F0[n];
            goto tail;
        }
        *(o->f1E8 + n) += delta;
        if (delta > 0) {
            func_8001F138(4);
        } else if (delta < 0) {
            func_8001F138(0x20);
        }
    }
tail:
    if (n != 8 && func_800301C8(o, n) != 0 && (u32)o->f11C < 4) {
        func_80010E24(o->f11C, 10);
    }
}
