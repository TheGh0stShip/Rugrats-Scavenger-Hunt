/* Target: func_80010850  VRAM 0x80010850  ROM 0x11450
 * Size: 0x19C  Compiler: SN GCC 2.8 (-O2)
 * Confidence: high
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef f32 Mtx_f[4][4];
typedef struct {
    f32 x, y, z;
} Vec3f;

struct Cam_80010850 {
    u8 pad_00[0x58];
    Vec3f unk_58;
    u8 pad_64[0xF4 - 0x64];
    s32 unk_F4;
    u8 pad_F8[0x100 - 0xF8];
    s32 unk_100;
    u8 pad_104[0x114 - 0x104];
    Vec3f unk_114;
};

extern s32 D_80052F18;
extern s32 D_80057D5C;
extern s32 func_8001BF00(void);
extern void func_80047790(Mtx_f m, f32 x, f32 y, f32 z);
extern void func_80047230(Mtx_f m, f32 a, f32 x, f32 y, f32 z);
extern void func_80046B70(Mtx_f a, Mtx_f b, Mtx_f out);
extern void func_80046C4C(Mtx_f m, f32 x, f32 y, f32 z, f32 *ox, f32 *oy, f32 *oz);
extern void func_8000FB08(struct Cam_80010850 *cam, Vec3f pos);

void func_80010850(struct Cam_80010850 *cam) {
    Vec3f unused;
    Mtx_f rot;
    Mtx_f m;
    Mtx_f trans;
    f32 zero;

    cam->unk_58 = cam->unk_114;
    if (cam->unk_F4 == 0) {
        cam->unk_100 = (cam->unk_100 + 1) % 360;
        if (D_80052F18 == 1 && func_8001BF00() != 0x1F) {
            if ((D_80057D5C - 1) * 90 + 75 < cam->unk_100 && cam->unk_100 < 0x159) {
                cam->unk_100 += 2;
            }
        }
    }
    zero = 0.0f;
    func_80047790(trans, cam->unk_114.x, zero, cam->unk_114.z);
    func_80047230(rot, (f32)cam->unk_100, zero, 1.0f, zero);
    func_80046B70(rot, trans, m);
    func_80046C4C(m, zero, zero, zero, &cam->unk_114.x, &cam->unk_114.y, &cam->unk_114.z);
    func_8000FB08(cam, cam->unk_114);
}
