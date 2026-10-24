/* Target: func_80012778  VRAM 0x80012778  ROM 0x13378
 * Size: 0x214  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Grid_8000DD88 {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
};

struct Obj_80012778 {
    u8 pad_00[0x2C];
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    u8 pad_38[0x13C - 0x38];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x151 - 0x13E];
    u8 unk151;
};

extern struct Grid_8000DD88 *func_8000DD88(u8);
extern u16 func_8000DE80(s32, s16, s16);
extern float sqrtf(float);

void func_80012778(struct Obj_80012778 *obj, u8 id) {
    struct Grid_8000DD88 *g;
    u8 center[2];
    s32 i;
    s32 j;
    f32 dx;
    f32 dy;
    f32 d;
    f32 best;

    g = func_8000DD88(id);
    center[1] = g->unk4 / 2;
    center[0] = g->unk6 / 2;
    best = 0.0f;
    for (i = 0; i < g->unk4; i++) {
        for (j = 0; j < g->unk6; j++) {
            {
                s16 tile = func_8000DE80(obj->unk151, i, j) & 0x7FFF;
                if (tile == 0 || (tile >= 0xF9 && tile <= 0xFC) || tile == 0xF5 || tile == 0xF6 || tile == 0xF7 || tile == 0xF8) {
                    continue;
                }
            }
            dx = center[0] - j;
            dy = center[1] - i;
            d = sqrtf(dx * dx + dy * dy);
            if (best < d) {
                best = d;
                obj->unk13C = j;
                obj->unk13D = i;
            }
        }
    }
    obj->unk2C = obj->unk13C * 600 + 300;
    obj->unk34 = obj->unk13D * 600 + 300;
    obj->unk13C += g->unk2;
    obj->unk13D += g->unk0;
}
