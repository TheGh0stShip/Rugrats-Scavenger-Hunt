/* Target: func_80006468  VRAM 0x80006468  ROM 0x7068
 * Size: 0xCC  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef float f32;

typedef struct Actor_80006468 {
    /* 0x000 */ u8 pad_000[0x2C];
    /* 0x02C */ f32 x;
    /* 0x030 */ f32 y;
    /* 0x034 */ f32 z;
    /* 0x038 */ u8 pad_038[0x13C - 0x38];
    /* 0x13C */ u8 gridX;
    /* 0x13D */ u8 gridZ;
    /* 0x13E */ u8 pad_13E[0x150 - 0x13E];
    /* 0x150 */ s16 area;
    /* 0x152 */ s16 prevArea;
} Actor_80006468;

typedef struct Rect_8000DD88 {
    /* 0x0 */ s16 z;
    /* 0x2 */ s16 x;
    /* 0x4 */ s16 d;
    /* 0x6 */ s16 w;
} Rect_8000DD88;

extern s32 func_8000E078(s32 z, s32 x);
extern Rect_8000DD88 *func_8000DD88(u8 id);

void func_80006468(Actor_80006468 *p, s32 x, s32 z) {
    s32 area = func_8000E078(z, x);
    u8 id = area;

    p->area = area;
    p->prevArea = area;
    p->gridX = x;
    p->gridZ = z;
    p->y = 0;
    p->x = (x - func_8000DD88(id)->x) * 600 + 300;
    p->z = (z - func_8000DD88(id)->z) * 600 + 300;
}
