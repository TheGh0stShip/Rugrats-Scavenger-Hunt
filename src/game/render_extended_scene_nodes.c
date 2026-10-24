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

struct Material_80014F00 {
    u8 pad_00[0x14];
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct NodeHdr_800149FC {
    s32 unk0;
    struct NodeHdr_800149FC *next;
    s32 unk8;
    void *unkC;
    u32 unk10;
    u32 unk14;
    void *unk18;
    void *unk1C;
    u32 unk20;
    Gfx *unk24;
    u8 pad_28[4];
    u16 unk2C;
    u16 unk2E;
    void *unk30;
    void *unk34;
};

extern Gfx *D_8007784C;
extern s32 D_80052A98;
extern s32 D_80052A90;
extern s32 D_80052EE8;
extern s32 D_80052EE4;
extern u32 D_80052EE0;
extern struct Fog_80052EEC D_80052EEC;
extern struct NodeHdr_800149FC *D_800581B4;
extern struct Alpha_800C33C0 D_800C33C0;
extern s32 D_80059DA8;
extern s32 D_80058C60;

extern void func_80008118(void *);
extern void func_80008174(void *);
extern void func_800081E0(void *);
extern void func_800149FC(void);
extern void func_80015740(void);
extern void func_8001420C(void *, u16, Gfx *);
extern void func_80014398(void *, u16, Gfx *);
extern void func_80014524(void *, u16, Gfx *);
extern void func_80014814(void *, u16, Gfx *);
extern u8 *func_800149A4(void *, u16, u8 *);
extern void func_80038D90(void *, const void *, s32);
extern void func_80047E00(void *, s32);
extern void func_8000CAB0(struct Material_80014F00 *, Gfx *, u16, void *);
extern u32 func_800495D0(void *);

#define GFX_W(hi, lo)              \
    {                              \
        Gfx *_g = D_8007784C++;    \
        _g->w0 = (hi);             \
        _g->w1 = (lo);             \
    }

void func_80014F00(void) {
    struct NodeHdr_800149FC *n;
    struct NodeHdr_800149FC *next;
    Gfx *var_s5;

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
    func_80008174(D_80059DA8);
    func_800081E0(D_80059DA8);
    func_800149FC();

    if (D_80052A98 == 0) {
        goto finish;
    }
    n = (struct NodeHdr_800149FC *)((u8 *)D_800581B4 + D_80052A90 * 56);
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
        s32 a;
        u32 flags = n->unk20;
        if (D_80052EE0 & 1) {
            a = 0x70;
        } else if (D_80052EE0 & 2) {
            a = 0x40;
        } else if (D_80052EE4 != 0) {
            if (flags & 0x10000) {
                s32 direction = 1;
                if (D_800C33C0.unk0 < 0x81) {
                    D_80058C60 = direction;
                } else if (D_800C33C0.unk0 >= 0xFF) {
                    direction = -1;
                    D_80058C60 = direction;
                }
                D_800C33C0.unk0 += D_80058C60 * 8;
                if (D_800C33C0.unk0 >= 0x100) {
                    D_800C33C0.unk0 = 255;
                } else if (D_800C33C0.unk0 < 0x80) {
                    D_800C33C0.unk0 = 0x80;
                }
                a = D_800C33C0.unk0;
            } else if (flags & 0x20000) {
                D_800C33C0.unk2 -= 15;
                if (D_800C33C0.unk2 < 75) {
                    D_800C33C0.unk2 = 75;
                }
                a = D_800C33C0.unk2;
            } else if (!(flags & 0x100000)) {
                a = D_800C33C0.unk4;
            } else {
                a = 0xFF;
            }
        } else {
            a = 0xFF;
        }
        a &= 0xFF;
        GFX_W(0x01020040, n->unk14);
        GFX_W(0xBC000C06, func_800495D0(n->unkC));
        GFX_W(0xBC001006, func_800495D0(n->unk18));
        if (flags & 0x07000000) {
            if (flags & 0x01000000) {
                flags |= 0x02000000;
                GFX_W(0xFA000000, (a << 24) | (a << 16) | (a << 8) | 0x64);
                GFX_W(0xE7000000, 0);
            } else if (flags & 0x04000000) {
                struct Material_80014F00 *material = n->unk1C;
                GFX_W(0xFA000000, (material->r << 24) | (material->g << 16) |
                                      (material->b << 8) | material->a);
                GFX_W(0xE7000000, 0);
                if (material->a != 0xFF) {
                    flags |= 0x02000000;
                }
            } else {
                GFX_W(0xFA000000, (a << 24) | (a << 16) | (a << 8) | 0xFF);
                GFX_W(0xE7000000, 0);
            }
            if (flags & 0x02000000) {
                u16 count = n->unk2C;
                void *references = n->unk30;
                u8 *found = func_800149A4(references, count, (u8 *)n->unk24);
                s32 size;
                if (found == 0) {
                    count = n->unk2E;
                    references = n->unk34;
                    found = func_800149A4(references, count, (u8 *)n->unk24);
                }
                size = found - (u8 *)n->unk24;
                func_80038D90(D_8007784C, n->unk24, size);
                func_80047E00(D_8007784C, size);
                var_s5 = D_8007784C;
                D_8007784C = (Gfx *)((u8 *)D_8007784C + size);
                func_8000CAB0(n->unk1C, var_s5, count, references);
            }
        } else {
            GFX_W(0xFA000000, (a << 24) | (a << 16) | (a << 8) | 0xFF);
            GFX_W(0xE7000000, 0);
        }
        if (!(flags & 0x03000000)) {
            GFX_W(0x06000000, n->unk10);
            var_s5 = n->unk24;
        }
        if ((flags & 0x01000000) ||
            ((flags & 0x04000000) &&
             ((struct Material_80014F00 *)n->unk1C)->a != 0xFF)) {
            func_80014814(n->unk30, n->unk2C, var_s5);
            func_80014814(n->unk34, n->unk2E, var_s5);
        } else if (flags & 0x08000000) {
            func_80014524(n->unk30, n->unk2C, var_s5);
            func_80014524(n->unk34, n->unk2E, var_s5);
        } else if (flags & 0x40000000) {
            func_80014398(n->unk30, n->unk2C, var_s5);
            func_80014398(n->unk34, n->unk2E, var_s5);
        } else {
            func_8001420C(n->unk30, n->unk2C, var_s5);
            func_8001420C(n->unk34, n->unk2E, var_s5);
        }
        GFX_W(0xB7000000, 0x2000);
        next = n;
        n->unk8 = 0;
        n->unk0 = 0;
        n = n->next;
        next->next = 0;
    } while (n != 0);
done:
    D_80052A98 = 0;
    D_80052A90 = 0;
    GFX_W(0xE7000000, 0);
    GFX_W(0xBA001402, 0);
finish:
    func_80015740();
}

