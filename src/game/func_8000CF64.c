/* Target: func_8000CF64  VRAM 0x8000CF64  ROM 0xDB64
 * Size: 0xE8  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef f32 MtxF[4][4];

typedef struct Cam_80052F04 {
    u8 pad_00[0x28];
    f32 at[3];    /* 0x28 */
    u8 pad_34[0x58 - 0x34];
    f32 eye[3];   /* 0x58 */
} Cam_80052F04;

extern Cam_80052F04 *D_80052F04;
extern void func_80047A38(void *mf, f32 a1, f32 a2, f32 a3, f32 f0, f32 f1, f32 f2, f32 f3);
extern void func_80047E00(void *mf, s32 flags);
extern void func_80045EBC(void *mf, f32 a1, f32 x, f32 y, f32 z);
extern void func_80046D00(void *a, void *b, void *dst);

void func_8000CF64(void *mf, f32 a1, f32 a2, f32 a3, f32 f0, f32 f1, f32 f2, f32 f3) {
    MtxF trans;
    MtxF rot;
    Cam_80052F04 *cam;

    func_80047A38(rot, 0.0f, 0.0f, 0.0f, f0, f1, f2, f3);
    func_80047E00(rot, 0x40);
    cam = D_80052F04;
    func_80045EBC(trans, 0.0f, cam->eye[0] - cam->at[0], cam->eye[1] - cam->at[1], cam->eye[2] - cam->at[2]);
    func_80047E00(trans, 0x40);
    func_80046D00(trans, rot, mf);
    func_80047E00(mf, 0x40);
}
