/* func_8002DA20  VRAM 0x8002DA20  ROM 0x2E620  Size: 0x178
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct Obj_8002E0E0 {
    u8 pad_000[0xFC];
    s32 fFC;            /* 0x0FC */
    s32 f100;           /* 0x100 */
    u8 pad_104[0x11C - 0x104];
    s32 f11C;           /* 0x11C */
    u8 pad_120[0x18C - 0x120];
    u32 flags;          /* 0x18C */
    s32 f190;           /* 0x190 */
    s16 timer;          /* 0x194 */
};

extern void func_8002DBCC(struct Obj_8002E0E0 *);
extern void func_8002DB98(struct Obj_8002E0E0 *);
extern s32 func_8001BF00(void);
extern s32 func_8003A840(void);
extern s16 D_800B92EC;
extern s32 D_80054560;

void func_8002DA20(struct Obj_8002E0E0 *o) {
    if (o->f11C != -1) {
        return;
    }
    o->flags = 0;
    if (o->timer > 0) {
        o->timer--;
    }
    if (o->f100 == 1) {
        func_8002DBCC(o);
    }
    if (o->fFC != 0) {
        if (o->fFC == 6) {
            func_8002DB98(o);
        }
        return;
    }
    switch (func_8001BF00()) {
        case 4:
            if (D_800B92EC != o->f190) {
                if (o->timer == 0) {
                    o->flags = 2;
                    o->timer = 30;
                }
            } else {
                o->flags = 0x10;
                if (D_800B92EC == 1) {
                    s32 r = func_8003A840();
                    D_80054560 = 0;
                    o->timer = r % 20 + 20;
                }
            }
            break;
        case 5:
            if (o->timer == 0 && D_80054560 == 0) {
                o->flags = 0x10;
                D_80054560 = 1;
            }
            break;
        case 37:
        case 43:
        case 44:
            if (o->timer == 0) {
                o->flags = 0x10;
            }
            break;
    }
}
