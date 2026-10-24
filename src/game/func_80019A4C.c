typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct { u32 w0; u32 w1; } Gfx;

struct Reloc_80019A4C {
    u16 index;
    u16 type;
    u8 pad_04[4];
};

void func_80019A4C(
    u8 *base, struct Reloc_80019A4C *relocs, u16 count, Gfx *gfx
) {
    u16 i;
    Gfx *g;
    u32 addr;
    u32 old;

    for (i = 0; i < count; i++) {
        g = &gfx[relocs[i].index];
        addr = (u32)base + g->w0;
        switch (relocs[i].type) {
        case 0:
            g->w0 = 0x06000000;
            g->w1 = addr;
            break;
        case 1:
            old = g->w1;
            g->w1 = addr;
            g->w0 = (((old << 1) & 0xFE) << 16)
                  | ((((old >> 16) << 10) | (((old >> 16) << 4) - 1)) & 0xFFFF)
                  | 0x04000000;
            break;
        }
    }
}
