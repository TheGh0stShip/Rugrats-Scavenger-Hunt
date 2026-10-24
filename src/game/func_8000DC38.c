typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Obj_800581F0 {
    s16 rows;
    s16 cols;
    u8 pad_04[0xE];
    s16 grid[1][64]; /* at 0x12, row stride 0x80 */
} Obj_800581F0;

extern Obj_800581F0 *D_800581F0;

s32 func_8000DC38(s32 id, u8 *outRow, u8 *outCol) {
    s32 r;
    s32 c;

    for (r = 0; r < D_800581F0->rows; r++) {
        for (c = 0; c < D_800581F0->cols; c++) {
            if (*(s16 *)((u8 *)D_800581F0 + r * 0x80 + c * 2 + 0x12) == id) {
                *outCol = c;
                *outRow = r;
                return 1;
            }
        }
    }
    return 0;
}
