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

struct Fog_80052EEC {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    s32 min;
    s32 max;
};

struct Alpha_800C33C0 {
    s16 unk0;
    s16 unk2;
    s16 unk4;
};

struct NodeHdr_800149FC {
    s32 unk0;
    struct NodeHdr_800149FC *next;
    s32 unk8;
    void *unkC;
    u32 unk10;
    u32 unk14;
    void *unk18;
    u8 pad_1C[4];
    u32 unk20;
    Gfx *unk24;
    u8 pad_28[4];
    u16 unk2C;
    u16 unk2E;
    void *unk30;
    void *unk34;
};

extern Gfx *D_8007784C;
extern s32 D_80052AB4;
extern s32 D_80052AAC;
extern s32 D_80052EE8;
extern s32 D_80052EE4;
extern u32 D_80052EE0;
extern struct Fog_80052EEC D_80052EEC;
extern struct NodeHdr_800149FC *D_800581B0;
extern struct Alpha_800C33C0 D_800C33C0;
extern s32 D_80059DA8;

extern void func_80008118(void *);
extern void func_800081E0(void *);
extern void func_8001420C(void *, u16, Gfx *);
extern u32 func_800495D0(void *);

#define GFX_W(hi, lo)              \
    {                              \
        Gfx *_g = D_8007784C++;    \
        _g->w0 = (hi);             \
        _g->w1 = (lo);             \
    }

void func_800149FC(void) {
    struct NodeHdr_800149FC *n;
    struct NodeHdr_800149FC *next;

    if (D_80052AB4 == 0) {
        return;
    }
    GFX_W(0xBB000001, 0x80008000);
    GFX_W(0xBA001301, 0x00080000);
    GFX_W(0xBA000C02, 0x2000);
    if (D_80052EE8 != 0) {
        GFX_W(0xB7000000, 0x00012204);
        GFX_W(0xB900031D, 0xC8112008);
        GFX_W(0xF8000000, (D_80052EEC.r << 24) | (D_80052EEC.g << 16) |
                              (D_80052EEC.b << 8) | D_80052EEC.a);
        GFX_W(0xBC000008,
              ((128000 / (D_80052EEC.max - D_80052EEC.min)) << 16) |
                  (((500 - D_80052EEC.min) * 256 / (D_80052EEC.max - D_80052EEC.min)) & 0xFFFF));
    } else {
        GFX_W(0xB7000000, 0x2204);
        GFX_W(0xB900031D, 0x0C192008);
    }
    GFX_W(0xFC127E03, 0xFF0FF3FF);
    GFX_W(0xBA001402, 0x00100000);
    func_80008118(D_80059DA8);
    func_800081E0(D_80059DA8);

    n = (struct NodeHdr_800149FC *)((u8 *)D_800581B0 + D_80052AAC * 56);
    if (D_80052EE4 != 0) {
        D_800C33C0.unk4 -= 15;
        if (D_800C33C0.unk4 < 75) {
            D_800C33C0.unk4 = 75;
        }
    }
    if (n == 0) {
        goto done;
    }
    do {
        u16 a;
        if (D_80052EE0 & 1) {
            a = 0x70;
        } else if (D_80052EE0 & 2) {
            a = 0x40;
        } else if (D_80052EE4 != 0) {
            if (n->unk20 & 0x10000) {
                D_800C33C0.unk0 += 15;
                if (D_800C33C0.unk0 >= 256) {
                    D_800C33C0.unk0 = 255;
                }
                a = D_800C33C0.unk0;
            } else if (n->unk20 & 0x20000) {
                D_800C33C0.unk2 -= 15;
                if (D_800C33C0.unk2 < 75) {
                    D_800C33C0.unk2 = 75;
                }
                a = D_800C33C0.unk2;
            } else if (!(n->unk20 & 0x100000)) {
                a = D_800C33C0.unk4;
            } else {
                a = 0xFF;
            }
        } else {
            a = 0xFF;
        }
        a &= 0xFF;
        GFX_W(0xFA000000, (a << 24) | (a << 16) | (a << 8) | 0xFF);
        GFX_W(0xE7000000, 0);
        func_8001420C(n->unk30, n->unk2C, n->unk24);
        func_8001420C(n->unk34, n->unk2E, n->unk24);
        GFX_W(0x01020040, n->unk14);
        GFX_W(0xBC000C06, func_800495D0(n->unkC));
        GFX_W(0xBC001006, func_800495D0(n->unk18));
        GFX_W(0x06000000, n->unk10);
        next = n;
        n->unk8 = 0;
        n->unk0 = 0;
        n = n->next;
        next->next = 0;
    } while (n != 0);
done:
    D_80052AB4 = 0;
    D_80052AAC = 0;
    GFX_W(0xE7000000, 0);
    GFX_W(0xBA001402, 0);
}

