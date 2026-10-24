/* Target: func_8000E6F8  VRAM 0x8000E6F8  ROM 0xF2F8
 * Size: 0xC8  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Cam_80052F04 {
    u8 pad_00[0x8];
    s16 unk08;
    s16 unk0A;
    u8 pad_0C[0x4];
    void *unk10;
    u8 pad_14[0x4];
    void *unk18;
    u8 pad_1C[0xC];
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    u8 pad_34[0x10];
    f32 unk44;
    u8 pad_48[0xFC - 0x48];
    s32 unkFC;
    s32 unk100;
    s32 unk104;
    u8 pad_108[0x148 - 0x108];
} Cam_80052F04;

typedef struct Blk_800586A8 {
    u32 words[0x148 / 4];
} Blk_800586A8;

extern void *func_8001A2B0(s32 size);
extern void *func_80038E90(void *p, s32 val, s32 size);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8000E7C8(void *);
extern void func_8000F994();
extern void *D_80059DA8;
extern Cam_80052F04 *D_80052F04;
extern Blk_800586A8 D_800586A8;

void func_8000E6F8(void) {
    Cam_80052F04 *cam = func_8001A2B0(sizeof(Cam_80052F04));

    func_80038E90(cam, 0, sizeof(Cam_80052F04));
    cam->unk08 = 1;
    cam->unk10 = func_8000E7C8;
    cam->unk18 = func_8000F994;
    cam->unkFC = -30;
    cam->unk0A = 0;
    cam->unk100 = 0;
    cam->unk104 = 1900;
    cam->unk44 = 1.0f;
    cam->unk28 = 50.0f;
    cam->unk2C = 50.0f;
    cam->unk30 = 50.0f;
    func_80007EFC(cam, D_80059DA8, 4, 0);
    D_80052F04 = cam;
    func_80038E90(&D_800586A8, -1, sizeof(Blk_800586A8));
}
