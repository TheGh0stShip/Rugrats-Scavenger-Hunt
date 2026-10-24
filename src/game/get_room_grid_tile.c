typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct RoomGrid {
    u8 pad_0000[0x2014];
    s16 row_offset;
    s16 column_offset;
};

extern u8 *D_800581F0;
extern s16 func_8000DF14(s32 column, s32 row);

s16 func_8000DE80(u8 room, s16 row, s16 column) {
    struct RoomGrid *grid = (struct RoomGrid *)(D_800581F0 + room * 0x210);
    register s32 adjusted_row __asm__("$7") = (s16)row;
    register s32 adjusted_column __asm__("$5") = (s16)column;

    adjusted_row = (s16)(adjusted_row + grid->row_offset);
    adjusted_column = (s16)(adjusted_column + grid->column_offset);
    return func_8000DF14(adjusted_row, adjusted_column);
}
