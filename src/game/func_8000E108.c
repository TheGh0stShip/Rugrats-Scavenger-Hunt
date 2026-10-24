/* Target: func_8000E108  VRAM 0x8000E108  ROM 0xED08
 * Size: 0x4C  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Obj_800581F0 {
    u8 pad_00[0xC];
    s16 unk0C;
    s16 unk0E;
} Obj_800581F0;

extern s32 D_80052F18;
extern Obj_800581F0 *D_800581F0;
extern s32 func_8000E078(s16 x, s16 y);

s32 func_8000E108(void) {
    if (D_80052F18 == 2) {
        return -1;
    }
    return func_8000E078(D_800581F0->unk0C, D_800581F0->unk0E);
}
