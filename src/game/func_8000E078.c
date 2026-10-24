typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

typedef struct Rect_800581F0 {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect_800581F0;

typedef struct Room_800581F0 {
    Rect_800581F0 rect;
    u8 pad_08[0x210 - 0x8];
} Room_800581F0;

typedef struct Map_800581F0 {
    u8 pad_00[0x4];
    s16 count;
    u8 pad_06[0x2014 - 0x6];
    Room_800581F0 rooms[1];
} Map_800581F0;

extern Map_800581F0 *D_800581F0;

s32 func_8000E078(s32 px, s32 py) {
    s32 i;

    for (i = 0; i < D_800581F0->count; i++) {
        Rect_800581F0 *r = &D_800581F0->rooms[i].rect;
        if (px >= r->x && px < r->x + r->w && py >= r->y && py < r->y + r->h) {
            goto found;
        }
    }
    return -1;
found:
    return i;
}
