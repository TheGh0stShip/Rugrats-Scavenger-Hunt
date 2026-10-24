/* func_8000F4E4  ROM 0x100E4  Size: 0x120  (camera orbit reset)
 * SN GCC 2.8 -O2. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

typedef f32 MtxF[4][4];

typedef struct Vec3f {
    f32 x, y, z;
} Vec3f;

typedef struct Ctl_800C33D8 {
    u8 pad_00[0xC];
    f32 unk0C;
    u8 pad_10[0x4];
    f32 unk14;
    s32 unk18;
    s32 unk1C;
} Ctl_800C33D8;

typedef struct Cam_80052F04 {
    u8 pad_00[0x58];
    Vec3f pos;          /* 0x58 */
    u8 pad_64[0xFC - 0x64];
    s32 unkFC;
    s32 unk100;
    s32 unk104;
    u8 pad_108[0xC];
    f32 unk114;
    s32 unk118;
    f32 unk11C;
} Cam_80052F04;

extern Vec3f D_800C33D8;
extern s32 D_800547C0;
extern s32 D_800547C4;
extern void func_80047790(void *mf, s32 x, f32 y, s32 z);
extern void func_80047230(void *mf, f32 angle, f32 x, f32 y, f32 z);
extern void func_80046B70(void *a, void *b, void *dst);
extern void func_80046C4C(void *mf, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz);
extern void func_8000FB08(Cam_80052F04 *cam, Vec3f pos);

void func_8000F4E4(Cam_80052F04 *cam) {
    Vec3f pad;
    MtxF rot;
    MtxF res;
    MtxF trans;

    cam->pos.x = D_800C33D8.x;
    cam->pos.z = D_800C33D8.z;
    cam->unk114 = D_800C33D8.x;
    cam->unk118 = 0;
    cam->unk11C = D_800C33D8.z;
    cam->unk100++;
    cam->unkFC = D_800547C0;
    cam->unk104 = D_800547C4;

    func_80047790(trans, *(s32 *)&cam->unk114, 0.0f, *(s32 *)&cam->unk11C);
    func_80047230(rot, cam->unk100, 0.0f, 1.0f, 0.0f);
    func_80046B70(rot, trans, res);
    func_80046C4C(res, 0.0f, 0.0f, 0.0f, &cam->unk114, (f32 *)&cam->unk118, &cam->unk11C);
    func_8000FB08(cam, cam->pos);
}
