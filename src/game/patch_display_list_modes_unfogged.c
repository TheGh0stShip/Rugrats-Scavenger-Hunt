typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Ref_800146B0 {
    u16 index;
    u16 type;
    u8 pad_04[4];
};

void func_800146B0(struct Ref_800146B0 *refs, u16 count, Gfx *dl) {
    u16 i;
    Gfx *g;

    for (i = 0; i < count; i++) {
        g = &dl[refs[i].index];
        switch (refs[i].type) {
            case 2:
                g->w0 = 0xFCFFFE03;
                g->w1 = 0xFF0E79FF;
                break;
            case 3:
            case 4:
                g->w0 = 0xFC127E03;
                g->w1 = 0xFF0FF3FF;
                break;
            case 5:
                g->w0 = 0xB900031D;
                g->w1 = 0x0C192008;
                break;
            case 6:
                g->w0 = 0xB900031D;
                g->w1 = 0x0C192038;
                break;
            case 7:
                g->w0 = 0xB900031D;
                g->w1 = 0x0C184240;
                break;
            case 8:
                g->w0 = 0xB900031D;
                g->w1 = 0x0C184A50;
                break;
            case 9:
            case 10:
                g->w0 = 0xB7000000;
                g->w1 = 1;
                break;
            case 11:
                g->w0 = 0xB6000000;
                g->w1 = 1;
                break;
            case 12:
                g->w0 = 0xB6000000;
                g->w1 = 0x2000;
                break;
        }
    }
}
