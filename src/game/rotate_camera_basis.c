typedef unsigned char u8;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

struct Cam_80008C24 {
    u8 pad_00[0xC];
    Vec3f up;
    Vec3f fwd;
    Vec3f right;
};

extern float sqrtf(float);

void func_80008C24(struct Cam_80008C24 *cam, f32 amount) {
    f32 len;

    cam->fwd.x += amount * cam->right.x;
    cam->fwd.y += amount * cam->right.y;
    cam->fwd.z += amount * cam->right.z;
    len = sqrtf(cam->fwd.x * cam->fwd.x + cam->fwd.y * cam->fwd.y + cam->fwd.z * cam->fwd.z);
    if (1e-7f <= len) {
        cam->fwd.x /= len;
        cam->fwd.y /= len;
        cam->fwd.z /= len;
    }
    cam->right.x = cam->up.y * cam->fwd.z - cam->up.z * cam->fwd.y;
    cam->right.y = cam->up.z * cam->fwd.x - cam->up.x * cam->fwd.z;
    cam->right.z = cam->up.x * cam->fwd.y - cam->up.y * cam->fwd.x;
    len = sqrtf(cam->right.x * cam->right.x + cam->right.y * cam->right.y + cam->right.z * cam->right.z);
    if (1e-7f <= len) {
        cam->right.x /= len;
        cam->right.y /= len;
        cam->right.z /= len;
    }
}
