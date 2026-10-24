
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
extern s32 D_800B92F4;

void func_8002E5B8(struct Obj_8002E0E0 *o) {
    s32 best = 0;
    s32 i;
    s32 idx;
    s32 d;

    for (i = 0; i < 4; i++) {
        if (func_800301C8(o, i) == 0) {
            { s32 a = o->f1E8[i]; d = o->f1F0[i] - a; }
            if (best < d) {
                best = d;
                idx = i;
            }
        }
    }
    D_800B92F4 = idx;
}
