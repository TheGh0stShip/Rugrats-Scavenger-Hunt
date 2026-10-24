/* Target: func_80010190  VRAM 0x80010190  ROM 0x10D90
 * Size: 0x1C8  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { f32 x, y, z; } Vec3f;

struct Cam_80052F04 {
    u8 pad_00[0x58];
    Vec3f pos;
    u8 pad_64[0xF0 - 0x64];
    s32 unk_F0;
    u8 pad_F4[0xFC - 0xF4];
    s32 unk_FC;
    s32 unk_100;
    s32 unk_104;
};

struct Player_80010190 {
    u8 pad_00[0x2C];
    f32 unk_2C;
    f32 unk_30;
    f32 unk_34;
    u8 pad_38[0x124 - 0x38];
    s32 unk_124;
};

extern struct Cam_80052F04 *D_80052F04;
extern int func_80005598(void *);
extern void func_8000FB08(struct Cam_80052F04 *cam, Vec3f pos);

void func_80010190(struct Player_80010190 *pl) {
    struct Cam_80052F04 *cam;

    D_80052F04->pos.x = pl->unk_2C;
    D_80052F04->pos.z = pl->unk_34;
    if (func_80005598(pl) != 0) {
        D_80052F04->unk_100 = (pl->unk_124 + 45) % 360;
    } else {
        switch (pl->unk_124) {
            case 0:
            case 360:
                D_80052F04->pos.z -= 200.0f;
                break;
            case 90:
                D_80052F04->pos.x -= 200.0f;
                break;
            case 180:
                D_80052F04->pos.z += 200.0f;
                break;
            case 270:
                D_80052F04->pos.x += 200.0f;
                break;
        }
        D_80052F04->unk_100 = (pl->unk_124 - 60) % 360;
    }
    cam = D_80052F04;
    cam->pos.y = 400.0f;
    cam->unk_FC = -10;
    cam->unk_104 = 0x546;
    func_8000FB08(cam, cam->pos);
    D_80052F04->unk_F0 = 0xC;
}
