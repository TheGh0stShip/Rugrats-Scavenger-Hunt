/* Target: func_80026560  VRAM 0x80026560  ROM 0x27160  Size: 0x1DC. */
typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct MergeRect {
    s16 x, y, w, h, type;
};

struct MergePattern {
    u8 pad_00[4];
    s32 stride;
    u8 *data[3];
};

struct MergeRow {
    u8 pad_00[0x12];
    s16 cells[0x37];
};

struct MergeMode {
    s32 mode;
    s32 frame;
};

extern struct MergePattern D_8005409C[];
extern s32 D_80054240[];
extern struct MergeMode D_80052F18;
extern struct MergeRow *D_800581F0;
extern struct MergeRect *func_8000DD88(u8 index);

void func_80026560(s32 index) {
    struct MergeRect *rect;
    u8 *source;
    s32 stride;
    s32 row;
    s32 column;
    u8 *entry;
    struct MergePattern *pattern;
    s16 *cells;
    s32 prior;

    if (D_80052F18.mode == 2) {
        return;
    }
    rect = func_8000DD88(index);
    if (rect->type == 0x53) {
        source = D_8005409C[0x53 - 0x47].data[D_80052F18.frame];
    } else {
        pattern = &D_8005409C[rect->type - 0x47];
        source = pattern->data[D_80054240[index]];
    }
    stride = D_8005409C[rect->type - 0x47].stride;
    for (row = rect->x; row < rect->x + rect->w; row++) {
        cells = D_800581F0[row].cells;
        for (column = rect->y; column < rect->y + rect->h; column++) {
            entry = &source[(row - rect->x) * stride + (column - rect->y)];
            if (*entry != 0) {
                prior = cells[column];
                cells[column] = *entry | (prior & 0x8000u);
            }
        }
    }
    D_80054240[index] = (D_80054240[index] + 1) % 3;
}
