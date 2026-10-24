/* func_8002DBD8  VRAM 0x8002DBD8  ROM 0x2E7D8  Size: 0x284
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Obj_8002E0E0 {
    u8 pad_000[0xF4];
    s32 fF4;            /* 0x0F4 */
    s32 fF8;            /* 0x0F8 */
    u8 pad_0FC[0x1E8 - 0xFC];
    s16 f1E8[4];        /* 0x1E8 current values */
    s16 f1F0[4];        /* 0x1F0 maximum values */
    s16 f1F8;           /* 0x1F8 */
};

extern struct Obj_8002E0E0 *func_800055A4(s32);
extern s32 func_800301C8(struct Obj_8002E0E0 *, s32);
extern s32 func_8003A840(void);
extern s32 D_80057D5C;
extern s32 D_80054564;   /* best score */
extern s32 D_800B92F0;   /* chosen item slot */
extern s32 D_800B92F8;   /* chosen target player */

void func_8002DBD8(struct Obj_8002E0E0 *self) {
    s32 p;
    s32 k;
    s32 mine;
    s32 theirs;
    s32 v;
    struct Obj_8002E0E0 *o;

    D_800B92F0 = 0;
    D_800B92F8 = 0;
    D_80054564 = -1;
    for (p = 0; p < D_80057D5C; p++) {
        o = func_800055A4(p);
        if (o->fF4 == self->fF4) {
            continue;
        }
        for (k = 0; k < 4; k++) {
            if (func_800301C8(self, k) != 0) {
                continue;
            }
            if (func_800301C8(o, k) != 0) {
                continue;
            }
            mine = self->f1E8[k];
            theirs = o->f1E8[k];
            if (theirs < mine) {
                continue;
            }
            if (self->f1F0[k] < theirs) {
                v = theirs - self->f1F0[k];
                v -= mine;
            } else {
                v = theirs - mine;
            }
            if (D_80054564 < v) {
                D_80054564 = v;
                D_800B92F0 = k;
                D_800B92F8 = p;
            }
        }
    }
    if (D_80054564 > 0) {
        return;
    }
    for (p = 0; p < D_80057D5C; p++) {
        o = func_800055A4(p);
        if (o->fF4 != self->fF4 && D_80054564 < 0) {
            D_80054564 = 0;
            D_800B92F0 = 4;
            D_800B92F8 = p;
        }
    }
    if (D_80054564 == -1) {
        do {
            p = func_8003A840() % 4;
        } while (p == self->fF4);
        do {
            k = func_8003A840() % 5;
        } while (k != 4 && (func_800301C8(self, k) != 0 || func_800301C8(o, k) != 0));
        D_800B92F8 = p;
        D_800B92F0 = k;
    }
}
