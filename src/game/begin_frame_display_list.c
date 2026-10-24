typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef float f32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Task_80013BC8 {
    u8 pad_00[0x68];
    Gfx dl[0x30];
    Gfx dlBody[1];
    u8 pad_1F0[0xE788 - 0x1F0];
    void *frameBuffer;
};

extern Gfx *D_80052F00;
extern Gfx *D_8007784C;
extern Gfx *D_80077830;
extern void *D_80052EFC;
extern s32 D_80052F08;
extern s32 D_80052EE8;
extern u8 D_80052EEC[3];
extern u64 D_80058CE8;
extern u8 D_80100000[];
extern Gfx D_01000128[];
extern Gfx D_01000010[];
extern Gfx D_01000100[];

extern void func_800336A4(void);
extern u64 func_80048A10(void);
extern u32 func_800495D0(void *);

#define GFX_W(hi, lo)              \
    {                              \
        Gfx *_g = D_8007784C++;    \
        _g->w0 = (hi);             \
        _g->w1 = (lo);             \
    }

#define RGBA5551(r, g, b, a) \
    ((((r) << 8) & 0xF800) | (((g) << 3) & 0x7C0) | (((b) >> 2) & 0x3E) | (a))

void func_80013BC8(struct Task_80013BC8 *t) {
    Gfx *dl = t->dl;

    D_80052F00 = dl;
    D_8007784C = t->dlBody;
    D_80077830 = (Gfx *)((u8 *)dl + 0xC180);
    func_800336A4();
    D_80058CE8 = func_80048A10();

    GFX_W(0xBC000006, 0);
    GFX_W(0xBC000406, func_800495D0(D_80052EFC));
    GFX_W(0x06000000, (u32)D_01000128);
    if (D_80052F08 != 0) {
        GFX_W(0x06000000, (u32)D_01000010);
        D_80052F08 = 0;
    }
    GFX_W(0x06000000, (u32)D_01000100);
    GFX_W(0xFE000000, func_800495D0(D_80100000));
    GFX_W(0xE7000000, 0);
    GFX_W(0xBA001402, 0x00300000);
    GFX_W(0xFF1001FF, func_800495D0(D_80100000));
    GFX_W(0xF7000000, 0xFFFCFFFC);
    GFX_W(0xF67FC3BC, 0);
    GFX_W(0xE7000000, 0);
    GFX_W(0xFF1001FF, func_800495D0(t->frameBuffer));
    GFX_W(0xBA000903, 0xC00);
    if (D_80052EE8 != 0) {
        GFX_W(0xF7000000,
              (RGBA5551(D_80052EEC[0], D_80052EEC[1], D_80052EEC[2], 1) << 16) |
                  RGBA5551(D_80052EEC[0], D_80052EEC[1], D_80052EEC[2], 1));
    } else {
        GFX_W(0xF7000000, 0x00010001);
    }
    GFX_W(0xF67FC3BC, 0);
    GFX_W(0xE7000000, 0);
    GFX_W(0xBA001402, 0);
}
