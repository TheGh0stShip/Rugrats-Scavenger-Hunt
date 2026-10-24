/* Target: func_8000FDAC  VRAM 0x8000FDAC  ROM 0x109AC  Size: 0x3E4. */
typedef unsigned char u8;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct CameraMode {
    u8 pad_00[0x58];
    Vec3f at;
    u8 pad_64[0xF0 - 0x64];
    s32 mode;
    u8 pad_F4[4];
    void *target;
    s32 pitch;
    s32 yaw;
    s32 distance;
};

struct CameraPlayer {
    u8 pad_00[0x2C];
    Vec3f pos;
    u8 pad_38[0x124 - 0x38];
    s32 angle;
};

extern struct CameraMode *D_80052F04;
extern void func_8000FB08(struct CameraMode *camera, Vec3f position);

void func_8000FDAC(s32 mode, struct CameraPlayer *player) {
    struct CameraMode *camera = D_80052F04;
    s32 dx, dy, dz;

    camera->mode = mode;
    switch (mode) {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            dz = dy = dx = 0;
            camera->at.x = player->pos.x;
            camera->at.y = player->pos.y;
            camera->at.z = player->pos.z;

            switch (mode) {
                case 5:
                    D_80052F04->pitch = 10;
                    D_80052F04->yaw = (player->angle - 110) % 360;
                    D_80052F04->distance = 0x546;
                    dy = 400;
                    switch (player->angle) {
                        case 0:
                        case 360:
                            D_80052F04->at.z += 200.0f;
                            break;
                        case 90:
                            D_80052F04->at.x += 200.0f;
                            break;
                        case 180:
                            D_80052F04->at.z -= 200.0f;
                            break;
                        case 270:
                            D_80052F04->at.x -= 200.0f;
                            break;
                    }
                    break;
                case 6:
                    D_80052F04->pitch = -30;
                    D_80052F04->yaw = (player->angle + 90) % 360;
                    D_80052F04->distance = 0xBB8;
                    break;
                case 7:
                    D_80052F04->pitch = -40;
                    dy = 0x514;
                    D_80052F04->yaw = player->angle % 360;
                    D_80052F04->distance = 0x1770;
                    D_80052F04->at.x = D_80052F04->at.x * 0.5;
                    D_80052F04->at.z = D_80052F04->at.z * 0.5;
                    break;
                case 8:
                    D_80052F04->pitch = -10;
                    dy = 0x12C;
                    D_80052F04->yaw = (player->angle + 30) % 360;
                    D_80052F04->distance = 0x4B0;
                    break;
                case 9:
                    D_80052F04->pitch = 0;
                    dy = 0x12C;
                    D_80052F04->yaw = (player->angle + 15) % 360;
                    D_80052F04->distance = 0xBB8;
                    break;
            }

            switch (player->angle) {
                case 0:
                    break;
                case 90:
                    dx = dz;
                    dz = -dz;
                    break;
                case 180:
                    dx = -dx;
                    dz = -dz;
                    break;
                case 270:
                    dx = dz;
                    break;
            }
            D_80052F04->at.x += dx;
            D_80052F04->at.y += dy;
            D_80052F04->at.z += dz;
            break;
        case 10:
            camera->at = player->pos;
            D_80052F04->at.y = 400.0f;
            D_80052F04->pitch = 0;
            D_80052F04->distance = 0x4B0;
            D_80052F04->yaw = player->angle;
            func_8000FB08(D_80052F04, D_80052F04->at);
            break;
        case 11:
            camera->target = player;
            break;
    }
}
