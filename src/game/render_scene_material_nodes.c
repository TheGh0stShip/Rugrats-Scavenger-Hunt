
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Mat_1C {
    u8 pad_00[0x14];
    u8 rgba[4];   /* 0x14..0x17 */
};

struct RNode_800581B4 {
    s32 unk0;
    struct RNode_800581B4 *next; /* 0x04 */
    s32 unk8;
    void *seg3;   /* 0x0C */
    void *dlist;  /* 0x10 */
    void *mtx;    /* 0x14 */
    void *seg4;   /* 0x18 */
    struct Mat_1C *mat; /* 0x1C */
    s32 flags;    /* 0x20 */
    u8 *data;     /* 0x24 */
    u8 *dataEnd0; /* 0x28 (unused here) */
    u16 unk2C;
    u16 unk2E;
    s32 unk30;
    s32 unk34;
}; /* size 0x38 */

struct Fog_80052EEC {
    u8 rgba[4];
    s32 near;    /* 0x04 */
    s32 far;     /* 0x08 */
};

extern Gfx *D_8007784C;
extern s32 D_80052EE0;
extern s32 D_80052EE4;
extern s32 D_80052EE8;
extern struct Fog_80052EEC D_80052EEC;
extern s32 D_80059DA8;
extern s32 D_80052A98;
extern s32 D_80052A90;
extern struct RNode_800581B4 *D_800581B4;
struct Alpha_800C33C0 {
    s16 unk0;
    s16 unk2;
    s16 unk4;
};
extern struct Alpha_800C33C0 D_800C33C0;
extern s32 D_80058C60;

extern void func_80008118(s32);
extern void func_80008174(s32);
extern void func_800081E0(s32);
extern void func_800149FC(void);
extern u32 func_800495D0(void *);
extern u8 *func_800149A4(s32, u16, u8 *);
extern void *func_80038D90(void *, void *, s32);
extern void func_80047E00(void *, s32);
extern void func_8000CAB0(struct Mat_1C *, void *, u16, s32);
extern void func_80014814(s32, u16, void *);
extern void func_80014524(s32, u16, void *);
extern void func_80014398(s32, u16, void *);
extern void func_8001420C(s32, u16, void *);
extern void func_80015740(void);

#define GFX_W(a, b)                       \
    {                                     \
        Gfx *_g = (Gfx *)(D_8007784C++);  \
        _g->w0 = (a);                     \
        _g->w1 = (b);                     \
    }

#define RGBA32(p) (((p)[0] << 24) | ((p)[1] << 16) | ((p)[2] << 8) | (p)[3])

void func_80014F00(void) {
    struct RNode_800581B4 *node;
    struct RNode_800581B4 *prev;
    s32 flags;
    s32 cnt;
    u8 *end;
    u8 *vtx;
    s32 size;

    GFX_W(0xBB000001, 0x80008000);
    GFX_W(0xBA001301, 0x00080000);
    GFX_W(0xBA000C02, 0x2000);
    if (D_80052EE8 != 0) {
        GFX_W(0xB7000000, 0x00012204);
        GFX_W(0xB900031D, 0xC8112008);
        GFX_W(0xF8000000, (D_80052EEC.rgba[0] << 24) | (D_80052EEC.rgba[1] << 16) |
                              (D_80052EEC.rgba[2] << 8) | D_80052EEC.rgba[3]);
        GFX_W(0xBC000008,
              ((128000 / (D_80052EEC.far - D_80052EEC.near)) << 16) |
                  (((500 - D_80052EEC.near) * 256 / (D_80052EEC.far - D_80052EEC.near)) & 0xFFFF));
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

    if (D_80052A98 != 0) {
        node = (struct RNode_800581B4 *)((u8 *)D_800581B4 + D_80052A90 * 0x38);
        if (D_80052EE4 != 0) {
            D_800C33C0.unk4 -= 15;
            if (D_800C33C0.unk4 < 75) {
                D_800C33C0.unk4 = 75;
            }
        }
        if (node == (void *)0) {
            goto done;
        }
        do {
            u16 level;
            flags = node->flags;
            if (D_80052EE0 & 1) {
                level = 0x70;
            } else if (D_80052EE0 & 2) {
                level = 0x40;
            } else if (D_80052EE4 != 0) {
                if (flags & 0x10000) {
                    if (D_800C33C0.unk0 < 0x81) {
                        D_80058C60 = 1;
                    } else if (D_800C33C0.unk0 >= 0xFF) {
                        D_80058C60 = -1;
                    }
                    D_800C33C0.unk0 += D_80058C60 * 8;
                    if (D_800C33C0.unk0 >= 0x100) {
                        D_800C33C0.unk0 = 0xFF;
                    } else if (D_800C33C0.unk0 < 0x80) {
                        D_800C33C0.unk0 = 0x80;
                    }
                    level = D_800C33C0.unk0;
                } else if (flags & 0x20000) {
                    D_800C33C0.unk2 -= 15;
                    if (D_800C33C0.unk2 < 75) {
                        D_800C33C0.unk2 = 75;
                    }
                    level = D_800C33C0.unk2;
                } else if (!(flags & 0x100000)) {
                    level = D_800C33C0.unk4;
                } else {
                    level = 0xFF;
                }
            } else {
                level = 0xFF;
            }

            GFX_W(0x01020040, (u32)node->mtx);
            GFX_W(0xBC000C06, func_800495D0(node->seg3));
            GFX_W(0xBC001006, func_800495D0(node->seg4));

            if (flags & 0x7000000) {
                if (flags & 0x1000000) {
                    flags |= 0x2000000;
                    GFX_W(0xFA000000, ((level & 0xFF) << 24) | ((level & 0xFF) << 16) | ((level & 0xFF) << 8) | 0x64);
                    GFX_W(0xE7000000, 0);
                } else if (flags & 0x4000000) {
                    {
                        u8 *c = node->mat->rgba;
                        GFX_W(0xFA000000, RGBA32(c));
                    }
                    GFX_W(0xE7000000, 0);
                    if (node->mat->rgba[3] != 0xFF) {
                        flags |= 0x2000000;
                    }
                } else {
                    GFX_W(0xFA000000, ((level & 0xFF) << 24) | ((level & 0xFF) << 16) | ((level & 0xFF) << 8) | 0xFF);
                    GFX_W(0xE7000000, 0);
                }
                if (flags & 0x2000000) {
                    u16 n = node->unk2C;
                    s32 id = node->unk30;
                    end = func_800149A4(id, n, node->data);
                    if (end == (void *)0) {
                        end = func_800149A4(node->unk34, node->unk2E, node->data);
                        n = node->unk2E;
                        id = node->unk34;
                    }
                    size = end - node->data;
                    func_80038D90(D_8007784C, node->data, size);
                    func_80047E00(D_8007784C, size);
                    vtx = (u8 *)D_8007784C;
                    D_8007784C = (Gfx *)(vtx + size);
                    func_8000CAB0(node->mat, vtx, n, id);
                }
            } else {
                GFX_W(0xFA000000, ((level & 0xFF) << 24) | ((level & 0xFF) << 16) | ((level & 0xFF) << 8) | 0xFF);
                GFX_W(0xE7000000, 0);
            }

            if (!(flags & 0x3000000)) {
                GFX_W(0x06000000, (u32)node->dlist);
                vtx = node->data;
            }

            if (flags & 0x1000000) {
                func_80014814(node->unk30, node->unk2C, vtx);
                func_80014814(node->unk34, node->unk2E, vtx);
            } else if (flags & 0x4000000) {
                if (node->mat->rgba[3] != 0xFF) {
                    func_80014814(node->unk30, node->unk2C, vtx);
                    func_80014814(node->unk34, node->unk2E, vtx);
                } else if (flags & 0x8000000) {
                    func_80014524(node->unk30, node->unk2C, vtx);
                    func_80014524(node->unk34, node->unk2E, vtx);
                } else if (flags & 0x40000000) {
                    func_80014398(node->unk30, node->unk2C, vtx);
                    func_80014398(node->unk34, node->unk2E, vtx);
                } else {
                    func_8001420C(node->unk30, node->unk2C, vtx);
                    func_8001420C(node->unk34, node->unk2E, vtx);
                }
            } else if (flags & 0x8000000) {
                func_80014524(node->unk30, node->unk2C, vtx);
                func_80014524(node->unk34, node->unk2E, vtx);
            } else if (flags & 0x40000000) {
                func_80014398(node->unk30, node->unk2C, vtx);
                func_80014398(node->unk34, node->unk2E, vtx);
            } else {
                func_8001420C(node->unk30, node->unk2C, vtx);
                func_8001420C(node->unk34, node->unk2E, vtx);
            }

            GFX_W(0xB7000000, 0x2000);
            prev = node;
            node->unk8 = 0;
            node->unk0 = 0;
            node = node->next;
            prev->next = (void *)0;
        } while (node != (void *)0);
    done:
        D_80052A98 = 0;
        D_80052A90 = 0;
        GFX_W(0xE7000000, 0);
        GFX_W(0xBA001402, 0);
    }
    func_80015740();
}
