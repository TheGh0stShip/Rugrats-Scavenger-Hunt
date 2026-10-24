/* Target: func_8000DDAC  VRAM 0x8000DDAC  ROM 0xE9AC
 * Size: 0xA0  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Ent_800581F0 {
    u8 pad_00[0x14];
    s16 x;
    u8 pad_16[0x210 - 0x16];
} Ent_800581F0;

typedef struct Obj_800581F0 {
    u8 pad_00[0x4];
    s16 count;
    u8 pad_06[0x2000 - 0x6];
    Ent_800581F0 ents[1];
} Obj_800581F0;

extern s8 D_800581E8;
extern s8 D_800581E9;
extern s16 *D_800581EC;
extern Obj_800581F0 *D_800581F0;
extern s32 D_80052F18;
extern s32 D_80052EE8;
extern void func_800278F4(void);

void func_8000DDAC(s32 idx) {
    D_800581E9 = idx;
    D_800581E8 = idx - 1;
    D_800581EC = &D_800581F0->ents[(u8)idx].x;
    func_800278F4();
    if (D_80052F18 == 1 && D_800581E9 + 1 != D_800581F0->count) {
        D_80052EE8 = D_80052F18;
    } else {
        D_80052EE8 = 0;
    }
}
