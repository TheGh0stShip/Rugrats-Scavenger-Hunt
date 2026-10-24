typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

typedef struct Ent_800581F0 {
    u8 pad_00[0x14];
    s16 x;
    s16 y;
    u8 pad_18[0x210 - 0x18];
} Ent_800581F0;

typedef struct Obj_800581F0 {
    u8 pad_00[0x2000];
    Ent_800581F0 ents[1];
} Obj_800581F0;

extern Obj_800581F0 *D_800581F0;
extern s32 func_8000E078(s32 px, s32 py);

s32 func_8000DCC8(s32 px, s32 py, s32 *outX, s32 *outY) {
    s32 i = func_8000E078(px, py);

    if (i < 0) {
        return -1;
    }
    *outX = px - D_800581F0->ents[i].x;
    *outY = py - D_800581F0->ents[i].y;
    return i;
}
