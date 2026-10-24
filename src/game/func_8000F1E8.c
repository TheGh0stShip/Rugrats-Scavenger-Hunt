/* Target: func_8000F1E8  VRAM 0x8000F1E8  ROM 0xFDE8
 * Size: 0xD0  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Key3s {
    s16 x, y, z;
    s16 pad;
} Key3s;

typedef struct Cam_80052F04 {
    u8 pad_00[0x28];
    f32 unk28[3];
    u8 pad_34[0x58 - 0x34];
    f32 unk58[3];
    u8 pad_64[0xE8 - 0x64];
    void *unkE8;
} Cam_80052F04;

extern Cam_80052F04 *D_80052F04;
extern void func_8000F2B8(void *anim, s32 frame, Key3s *out0, Key3s *out1);
extern void func_8000F9D4(void *obj, f32 x, f32 y, f32 z, f32 *dst);

void func_8000F1E8(s32 frame, void *obj) {
    Key3s a;
    Key3s b;

    func_8000F2B8(D_80052F04->unkE8, frame, &a, &b);
    func_8000F9D4(obj, a.x, a.y, a.z, D_80052F04->unk28);
    func_8000F9D4(obj, b.x, b.y, b.z, D_80052F04->unk58);
}
