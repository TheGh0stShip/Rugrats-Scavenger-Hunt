/* Target: func_80015740  VRAM 0x80015740  ROM 0x16340
 * Size: 0x58C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3
 * Confidence: medium */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

typedef struct {
    s32 m[4][4];
} Mtx;


struct RNode_800581B8 {
    s32 unk0;
    struct RNode_800581B8 *next;
    s32 unk8;
    void *seg3;
    void *dlist;
    Mtx *mtx;
    void *seg4;
    u8 pad_1C[0x8];
    s32 unk24;
    s32 unk28;
    u16 unk2C;
    u16 unk2E;
    s32 unk30;
    s32 unk34;
};

struct FrameBuf_8009C358 {
    u8 pad_0000[0xE788];
    void *cfb;
    u8 pad_E78C[0x4];
};

extern Gfx *D_8007784C;
extern s32 D_80052AA4;
extern s32 D_80052AA8;
extern s32 D_80052A9C;
extern struct RNode_800581B8 *D_800581B8;
extern struct FrameBuf_8009C358 D_8009C358[];
extern s32 D_80054358;
extern Mtx D_80058C68;
extern Mtx D_80058CA8;
extern u8 D_80100000[];
extern f32 D_8000126C;
extern f32 D_80001270;
extern f32 D_80001274;
extern f32 D_80001278;

extern u32 func_800495D0(void *);
extern void func_800461E0(f32 (*)[4], f32, f32, f32, f32, f32, f32, f32, f32, f32);
extern void func_800468F0(f32 (*)[4], Mtx *);
extern void func_80047E00(void *, s32);
extern void func_80047060(Mtx *, u16 *, f32, f32, f32, f32, f32);
extern void func_800146B0(void *, u16, void *);

#define GFX_W(a, b)                       \
    {                                     \
        Gfx *_g = (Gfx *)(D_8007784C++);  \
        _g->w0 = (a);                     \
        _g->w1 = (b);                     \
    }

void func_80015740(void) {
    f32 mf[2][4][4];
    register f32 one asm("$f20");
    u16 perspNorm;
    struct RNode_800581B8 *node;
    struct RNode_800581B8 *prev;

    if (D_80052AA4 == 0 && D_80052AA8 == 0) {
        return;
    }

    GFX_W(0xE7000000, 0);
    GFX_W(0xB6000000, 0x1);
    GFX_W(0xB900031D, 0x0F0A4000);
    GFX_W(0xFE000000, func_800495D0(D_80100000));
    GFX_W(0xE7000000, 0);
    GFX_W(0xBA001402, 0x00300000);
    GFX_W(0xFF1001FF, func_800495D0(D_80100000));
    GFX_W(0xF7000000, 0xFFFCFFFC);
    GFX_W(0xF67FC3BC, 0);
    GFX_W(0xE7000000, 0);
    GFX_W(0xFF1001FF, func_800495D0(D_8009C358[D_80054358].cfb));
    GFX_W(0xE7000000, 0);
    GFX_W(0xF8000000, 0xFFFFFFFF);
    GFX_W(0xBC000008, 0xF4000D00);
    GFX_W(0xBA001402, 0x00100000);
    GFX_W(0xBB000001, 0x80008000);
    GFX_W(0xB6000000, 0x00010000);
    GFX_W(0xB7000000, 0x2205);
    GFX_W(0xBA001301, 0x00080000);
    GFX_W(0xBA000C02, 0x2000);
    GFX_W(0xFC127E03, 0xFF0FF3FF);
    GFX_W(0xB900031D, 0x0C192008);

    node = (struct RNode_800581B8 *)((u8 *)D_800581B8 + D_80052A9C * 0x38);

    if (D_80052AA4 != 0 || D_80052AA8 != 0) {
        one = D_8000126C;
        func_800461E0(mf[0], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, D_80001270, 0.0f, one, 0.0f);
        func_800468F0(mf[0], &D_80058C68);
        func_80047E00(&D_80058C68, 0x40);
        func_80047060(&D_80058CA8, &perspNorm, 20.0f, 2.1333334f, D_80001274, D_80001278, one);
        func_80047E00(&D_80058CA8, 0x40);
        GFX_W(0xBC00000E, perspNorm);
        GFX_W(0x01030040, (u32)&D_80058CA8);
        GFX_W(0x01010040, (u32)&D_80058C68);

        while (node != (void *)0) {
            if (D_80052AA4 == 0) {
                break;
            }
            GFX_W(0xFA000000, 0xFFFFFFFF);
            func_800146B0(node->unk30, node->unk2C, node->unk24);
            func_800146B0(node->unk34, node->unk2E, node->unk28);
            GFX_W(0xE7000000, 0);
            GFX_W(0x01020040, (u32)node->mtx);
            GFX_W(0xBC000C06, func_800495D0(node->seg3));
            GFX_W(0xBC001006, func_800495D0(node->seg4));
            GFX_W(0x06000000, (u32)node->dlist);
            prev = node;
            node->unk8 = 0;
            node->unk0 = 0;
            node = node->next;
            prev->next = (void *)0;
        }
    }

    D_80052AA8 = 0;
    D_80052AA4 = 0;
    D_80052A9C = 0;
    GFX_W(0xE7000000, 0);
}
