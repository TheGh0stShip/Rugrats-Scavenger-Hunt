/* Target: func_8000F604  VRAM 0x8000F604  ROM 0x10204  Size: 0x1FC. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;
typedef float f32;

struct CameraFollow {
    u8 pad_00[0x58];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_64[0xFC - 0x64];
    s32 angle_offset;
    s32 angle;
    s32 distance;
};

struct CameraTarget {
    u8 pad_00[0x2C];
    f32 x;
    f32 y;
    f32 z;
    u8 pad_38[0x11C - 0x38];
    s32 tile;
    u8 pad_120[4];
    s32 angle;
    u8 pad_128[0x1FC - 0x128];
    s32 side;
};

struct CameraTile {
    u16 id;
    u16 flags;
    u8 pad_4[6];
};

extern struct CameraTile D_80058B58[];
extern s32 D_80052BF0;
extern s32 D_80052F18;
extern s32 D_80052BE8;
extern int func_8001F588(s32 arg0);
extern void func_800137E8(s32 arg0);
extern void func_80013724(void);

void func_8000F604(struct CameraFollow *camera, struct CameraTarget *target) {
    s32 remainder;
    s32 delta;

    camera->x = target->x;
    camera->z = target->z;
    if (target->tile != -1) {
        u16 flags = D_80058B58[target->tile].flags;

        if (D_80052BF0 != 0) {
            D_80052BF0--;
        }
        if ((flags & 0x10) && D_80052F18 != 2 && D_80052BF0 == 0) {
            target->side = (target->side + 1) & 1;
            if (target->side == 0) {
                camera->angle_offset = -0x12;
            } else if (target->side == 1) {
                camera->angle_offset = -0x12;
            }
            D_80052BF0 = 4;
        }
        func_8001F588(0);
        func_800137E8(8);
        func_80013724();
        switch (target->side) {
            case 0:
                if (D_80052F18 == 2) {
                    camera->angle = target->angle + 0x6E;
                } else {
                    camera->angle = target->angle + 0xA5;
                }
                break;
            case 1:
                camera->angle = 0x159;
                break;
        }
        remainder = camera->angle % 360;
        delta = 180 - remainder;
        if (0.0f <= (f32)delta) {
            camera->distance = (D_80052BE8 * delta) / 180 + 0x960;
        } else {
            camera->distance = (D_80052BE8 * (remainder - 180)) / 180 + 0x960;
        }
    } else {
        camera->angle = target->angle;
        camera->distance = 0x960;
    }
}
