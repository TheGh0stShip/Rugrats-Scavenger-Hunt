/* Target: func_800065FC  VRAM 0x800065FC  ROM 0x71FC
 * Size: 0x224  Compiler: SN GCC 2.8 -O2 -G0 (reviewed loop-constant normalization), .rdata jump table at 0x80000540
 * Confidence: high
 */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

typedef struct Actor_800065FC {
    /* 0x000 */ u8 pad_000[0x124];
    /* 0x124 */ s32 unk_124;
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ u8 pad_12C[0x13C - 0x12C];
    /* 0x13C */ u8 gridX;
    /* 0x13D */ u8 gridZ;
} Actor_800065FC;

typedef struct Rect_8000DD88 {
    /* 0x0 */ s16 z;
    /* 0x2 */ s16 x;
    /* 0x4 */ s16 d;
    /* 0x6 */ s16 w;
} Rect_8000DD88;

extern s16 func_8000DF14(s16 z, s16 x);
extern s16 func_8000DF40(s16 tile);
extern int func_8000DC38(int, unsigned char *, unsigned char *);
extern s32 func_800058FC(u8 z, u8 x);
extern void func_8000798C(Actor_800065FC *p, s32 dz, s32 dx);
extern Rect_8000DD88 *func_8000DD88(u8 id);
extern void func_8000DEEC(s16 z, s16 x, s32 v);
extern s32 func_8000E078(u8 z, u8 x);
extern void func_80005528(Actor_800065FC *p, s32 v);

void func_800065FC(Actor_800065FC *p) {
    s32 tile;
    s32 id;
    Rect_8000DD88 *r;
    s32 i;
    s32 j;

    tile = func_8000DF40(func_8000DF14(p->gridZ, p->gridX));
    switch (tile) {
        case 0xF9:
            id = 0xFA;
            goto warp;
        case 0xFA:
            id = 0xF9;
            goto warp;
        case 0xFB:
            id = 0xFC;
            goto warp;
        case 0xFC:
            id = 0xFB;
        warp:
            func_8000DC38(id, &p->gridZ, &p->gridX);
            p->unk_128 = p->unk_124 = func_800058FC(p->gridZ, p->gridX);
            break;
        case 0xF7:
            func_8000798C(p, 0, -1);
            break;
        case 0xF8:
            func_8000798C(p, 0, 1);
            break;
        case 0xF6:
            func_8000798C(p, -1, 0);
            break;
        case 0xF5:
            func_8000798C(p, 1, 0);
            break;
        default:
            tile -= 20;
            if ((u32)tile < 5) {
                r = func_8000DD88(tile);
                for (i = r->z; i < r->z + r->d; i++) {
                    for (j = r->x; j < r->x + r->w; j++) {
                        if (func_8000DF40(func_8000DF14(i, j)) == 2) {
                            func_8000DEEC(i, j, 2);
                            p->gridX = j;
                            p->gridZ = i;
                        }
                    }
                }
            }
            break;
    }
    func_80005528(p, func_8000E078(p->gridZ, p->gridX));
}
