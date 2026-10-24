/* func_8002F428  VRAM 0x8002F428  ROM 0x30028  Size: 0xDC
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

extern struct Obj_8002E0E0 *func_800055A4(s32);
extern int func_8001F138(s32);
extern s32 D_800B9310;
extern s32 D_80057D5C;

s32 func_8002F428(s32 target) {
    s32 done;
    s32 i;
    struct Obj_8002E0E0 *o;
    s32 v;

    if (--D_800B9310 > 0) {
        return 0;
    }
    done = 1;
    D_800B9310 = 5;
    for (i = 0; i < D_80057D5C; i++) {
        o = func_800055A4(i);
        v = o->f1F8;
        if (v < target) {
            o->f1F8++;
        } else if (target < v) {
            o->f1F8--;
        }
        if (v != target) {
            done = 0;
        }
    }
    if (done == 1) {
        return 1;
    }
    func_8001F138(4);
    return 0;
}
