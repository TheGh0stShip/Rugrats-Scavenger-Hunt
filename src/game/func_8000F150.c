typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef struct Vec3f {
    f32 x, y, z;
} Vec3f;

typedef struct Cam_80052F04 {
    u8 pad_00[0x58];
    Vec3f pos;          /* 0x58 */
    u8 pad_64[0xF0 - 0x64];
    s32 unkF0;
    u8 pad_F4[0x8];
    s32 unkFC;
    s32 unk100;
    s32 unk104;
} Cam_80052F04;

typedef struct Src_8000F150 {
    u8 pad_00[0x2C];
    Vec3f pos;          /* 0x2C */
    u8 pad_38[0xF4 - 0x38];
    s32 unkF4;
    u8 pad_F8[0x124 - 0xF8];
    s32 unk124;
} Src_8000F150;

extern Cam_80052F04 *D_80052F04;
extern s32 D_80057D5C;
extern void func_8000FB08(Cam_80052F04 *cam, Vec3f pos);

void func_8000F150(Src_8000F150 *src) {
    Cam_80052F04 *cam;

    D_80052F04->pos = src->pos;
    cam = D_80052F04;
    cam->pos.y = 400.0f;
    cam->unkFC = 0;
    cam->unk100 = src->unk124;
    cam->unk104 = 1000;
    if (src->unkF4 < D_80057D5C) {
        cam->unkF0 = 12;
    } else {
        cam->unkF0 = 1;
    }
    func_8000FB08(D_80052F04, D_80052F04->pos);
}
