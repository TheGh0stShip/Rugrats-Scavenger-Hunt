/* Target: func_8003387C  VRAM 0x8003387C  ROM 0x3447C
 * Size: 0x7C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

typedef struct Task_800C3D68 {
    u8 pad_00[0x8];
    s16 unk08;   /* 0x08 */
    s16 unk0A;   /* 0x0A */
    s32 unk0C;   /* 0x0C */
    void *unk10; /* 0x10 */
    u8 pad_14[0x4];
    void *unk18; /* 0x18 */
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;   /* 0x34 */
} Task_800C3D68;
extern Task_800C3D68 D_800C3D68;
extern void *D_80059DA8;
extern void func_80033970(void *);
extern void func_80033E0C(void);
extern void func_80007EFC(void *, void *, int, void *);

void func_8003387C(void) {
    Task_800C3D68 *t = &D_800C3D68;

    t->unk08 = 2;
    t->unk10 = func_80033970;
    t->unk18 = func_80033E0C;
    t->unk34 = 2;
    t->unk0A = 0;
    t->unk0C = 0;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk24 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk30 = 0;
    func_80007EFC(t, D_80059DA8, 4, 0);
}
