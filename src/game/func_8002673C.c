/* Target: func_8002673C  VRAM 0x8002673C  ROM 0x2733C  Size: 0xE8  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Task_8002673C {
    u8 pad_00[0x8];
    s16 unk08;
    u8 pad_0A[0xA];
    void (*unk14)();
    void (*unk18)();
    u8 pad_1C[0xF8 - 0x1C];
};
struct Grid_800581F0 {
    u8 pad_00[0x4];
    s16 unk04;
};
extern struct Task_8002673C *D_8006FBF0;
extern s32 D_80059DA8;
extern s32 D_80052F18;
extern s32 D_80052EE4;
extern struct Grid_800581F0 *D_800581F0;
extern u8 D_800C33D8[];
extern void *func_8001A2B0(s32);
extern void *func_80038E90(void *, s32, s32);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8002637C(s32);
extern void func_80026824();
extern void func_80027938();

void func_8002673C(void) {
    struct Task_8002673C *t;
    s32 i;

    t = func_8001A2B0(0xF8);
    func_80038E90(t, 0, 0xF8);
    t->unk14 = func_80026824;
    t->unk18 = func_80027938;
    t->unk08 = 5;
    D_8006FBF0 = t;
    func_80007EFC(t, D_80059DA8, 4, 0);
    if (D_80052F18 != 2) {
        for (i = 0; i < D_800581F0->unk04; i++) {
            func_8002637C(i);
        }
    }
    D_80052EE4 = 0;
    func_80038E90(D_800C33D8, 0, 0x3C);
}
