/* Target: func_80012A14  VRAM 0x80012A14  ROM 0x13614
 * Size: 0x188  Compiler: KMC GCC 2.7.2 (-O2)
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

struct Obj_80012A14 {
    u8 pad_00[0x13C];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x151 - 0x13E];
    u8 unk151;
};

struct Globals_80058C00 {
    void *unk0;
    u8 pad_04[0x08];
    u8 unkC;
    u8 unkD;
};

extern struct Globals_80058C00 D_80058C00;

extern struct Grid_8000DD88 *func_8000DD88(s32);
extern u16 func_8000DE80(s32, s16, s16);
extern void func_80005500(s32, struct Obj_80012A14 *);
extern float sqrtf(float);

void func_80012A14(struct Obj_80012A14 *obj) {
    struct Grid_8000DD88 *g;
    s32 i;
    s32 j;
    f32 dx;
    f32 dy;
    f32 d;
    f32 best;
    u8 pos[2];

    i = 0;
    g = func_8000DD88(obj->unk151);
    best = 0.0f;
    for (i = 0; i < g->unk4; i++) {
        for (j = 0; j < g->unk6; j++) {
            {
                s16 tile = func_8000DE80(obj->unk151, i, j) & 0x7FFF;
                if (tile == 0 || (tile >= 0xF9 && tile <= 0xFC) || tile == 0xF5 || tile == 0xF6 || tile == 0xF7 || tile == 0xF8) {
                    continue;
                }
            }
            dx = obj->unk13C - (j + g->unk2);
            dy = obj->unk13D - (i + g->unk0);
            d = sqrtf(dx * dx + dy * dy);
            if (best < d) {
                D_80058C00.unkC = g->unk2 + j;
                best = d;
                D_80058C00.unkD = g->unk0 + i;
            }
        }
    }
    func_80005500(6, obj);
}
