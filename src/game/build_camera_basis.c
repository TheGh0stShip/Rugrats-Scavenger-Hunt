typedef int s32;
typedef float f32;

typedef struct {
    f32 x, y, z;
} Vec3f;

extern float sqrtf(float);

s32 func_8000893C(Vec3f *eye, Vec3f *at, Vec3f *up, Vec3f *fwd, Vec3f *right, Vec3f *newUp) {
    f32 len;

    fwd->x = eye->x - at->x;
    fwd->y = eye->y - at->y;
    fwd->z = eye->z - at->z;
    len = sqrtf(fwd->x * fwd->x + fwd->y * fwd->y + fwd->z * fwd->z);
    if (1e-7f <= len) {
        fwd->x /= len;
        fwd->y /= len;
        fwd->z /= len;
    }
    if (len < 1e-7f) {
        return 0;
    }

    right->x = up->y * fwd->z - up->z * fwd->y;
    right->y = up->z * fwd->x - up->x * fwd->z;
    right->z = up->x * fwd->y - up->y * fwd->x;
    len = sqrtf(right->x * right->x + right->y * right->y + right->z * right->z);
    if (1e-7f <= len) {
        right->x /= len;
        right->y /= len;
        right->z /= len;
    }
    if (len < 1e-7f) {
        return 0;
    }

    newUp->x = fwd->y * right->z - fwd->z * right->y;
    newUp->y = fwd->z * right->x - fwd->x * right->z;
    newUp->z = fwd->x * right->y - fwd->y * right->x;
    return 1;
}
