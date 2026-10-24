/* Target: func_8002637C  VRAM 0x8002637C  ROM 0x26F7C  Size: 0x1E4. */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct PatternRect {
    s16 x, y, w, h, type;
};

struct BoardPattern {
    u8 pad_00[4];
    s32 stride;
    u8 *data[3];
};

struct BoardRow {
    u8 pad_00[0x12];
    s16 cells[0x37];
};

struct BoardMode {
    s32 mode;
    s32 frame;
};

extern struct BoardPattern D_8005409C[];
extern s32 D_80054240[];
extern struct BoardMode D_80052F18;
extern struct BoardRow *D_800581F0;
extern struct PatternRect *func_8000DD88(u8 index);

void func_8002637C(s32 index) {
    struct PatternRect *rect;
    u8 *source;
    s32 stride;
    s32 row;
    s32 column;
    s32 value;
    s16 *cells;

    if (D_80052F18.mode == 2) {
        return;
    }
    rect = func_8000DD88(index);
    if (rect->type == 0x53) {
        source = D_8005409C[0x53 - 0x47].data[D_80052F18.frame];
    } else {
        source = D_8005409C[rect->type - 0x47].data[D_80054240[index]];
    }
    stride = D_8005409C[rect->type - 0x47].stride;
    for (row = rect->x; row < rect->x + rect->w; row++) {
        cells = D_800581F0[row].cells;
        for (column = rect->y; column < rect->y + rect->h; column++) {
            value = source[(row - rect->x) * stride + (column - rect->y)];
            if (value != 0) {
                cells[column] = value;
                if (value != 11) {
                    cells[column] = value | 0x8000;
                }
            }
        }
    }
    D_80054240[index] = (D_80054240[index] + 1) % 3;
}
