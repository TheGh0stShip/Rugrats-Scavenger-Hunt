/* func_80014398: patch render-mode/combine words in a display list from a type table. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct {
    u32 w0;
    u32 w1;
} Gfx;

struct Ref_80014398 {
    u16 index;
    u16 type;
    u8 pad_04[4];
};

extern s32 D_80052EE8;

void func_80014398(struct Ref_80014398 *refs, u32 count, Gfx *dl) {
    u16 i;
    Gfx *g;
    s32 mode;
    u32 w1;
    for (i = 0; i < (u16)count; i++) {
        g = &dl[refs[i].index];
        switch (refs[i].type) {
            case 2:
                g->w0 = 0xFCFFFE03;
                g->w1 = 0xFF0E79FF;
                break;
            case 3:
                g->w0 = 0xFC127E03;
                g->w1 = 0xFF0FF3FF;
                break;
            case 4:
                g->w0 = 0xFC127E03;
                g->w1 = 0xFF0FF3FF;
                break;
            case 5:
                if (D_80052EE8) {
                    w1 = 0xC8112008;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                } else {
                    w1 = 0x0C192008;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                }
                break;
            case 6:
                if (D_80052EE8) {
                    w1 = 0xC8112038;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                } else {
                    w1 = 0x0C192038;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                }
                break;
            case 7:
                if (D_80052EE8) {
                    w1 = 0xC8104240;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                } else {
                    w1 = 0x0C184240;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                }
                break;
            case 8:
                if (D_80052EE8) {
                    w1 = 0xC8104A50;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                } else {
                    w1 = 0x0C184A50;
                    g->w0 = 0xB900031D;
                    g->w1 = w1;
                }
                break;
            case 9:
                g->w0 = 0xB7000000;
                g->w1 = 1;
                break;
            case 10:
                g->w0 = 0xB7000000;
                g->w1 = 1;
                break;
            case 11:
                g->w0 = 0xB6000000;
                g->w1 = 1;
                break;
            case 12:
                w1 = 0x2000;
                g->w0 = 0xB6000000;
                g->w1 = w1;
                break;
        }
    }
}
