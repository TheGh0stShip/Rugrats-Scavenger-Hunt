typedef unsigned char u8;
typedef int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Cam_80008448 {
    u8 pad_00[0x4];
    Vec3f eye;
    Vec3f at;
    Vec3f dir;
    f32 dist;
    u8 pad_2C[0xC];
    f32 unk38;
};

extern float sqrtf(float);

s32 func_80008448(struct Cam_80008448 *cam) {
    f32 len;
    f32 t;

    cam->dir.x = cam->at.x - cam->eye.x;
    cam->dir.y = cam->at.y - cam->eye.y;
    cam->dir.z = cam->at.z - cam->eye.z;
    t = cam->dist - cam->unk38;
    len = sqrtf(cam->dir.x * cam->dir.x + cam->dir.y * cam->dir.y + cam->dir.z * cam->dir.z);
    cam->dist = len;
    if (0.0f < cam->dist) {
        cam->dir.x /= cam->dist;
        cam->dir.y /= cam->dist;
        cam->dir.z /= cam->dist;
        if (t < cam->dist) {
            cam->dist = t;
            cam->at.x = cam->eye.x + cam->dir.x * t;
            cam->at.y = cam->eye.y + cam->dir.y * t;
            cam->at.z = cam->eye.z + cam->dir.z * t;
        }
        return 0;
    }
    return -1;
}
