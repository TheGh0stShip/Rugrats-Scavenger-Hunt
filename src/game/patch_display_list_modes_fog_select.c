typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Ref_80014814 {
    u16 index;
    u16 type;
    u8 pad_04[4];
};

extern s32 D_80052EE8;

void func_80014814(struct Ref_80014814 *refs, u16 count, Gfx *dl) {
    u16 i;
    Gfx *g;
    s32 mode;
    u32 w1;

    i = 0;
    if (count != 0) {
    mode = D_80052EE8;
    do {
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
                g->w1 = mode ? 0xC8104240 : 0x0C184240;
                break;
            case 6:
                g->w0 = 0xB900031D;
                g->w1 = mode ? 0xC8104A50 : 0x0C184A50;
                break;
            case 7:
                g->w0 = 0xB900031D;
                g->w1 = mode ? 0xC8104240 : 0x0C184240;
                break;
            case 8:
                g->w0 = 0xB900031D;
                g->w1 = mode ? 0xC8104A50 : 0x0C184A50;
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
        i++;
    } while (i < count);
    }
}
