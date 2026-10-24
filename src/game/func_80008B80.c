typedef int s32;
typedef float f32;
typedef double f64;

typedef struct {
    f32 x, y, z;
} Vec3f;

extern f64 D_80000620;
extern float sqrtf(float);

s32 func_80008B80(Vec3f a, Vec3f b) {
    f32 d;

    d = sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    if (d < D_80000620) {
        d = 0.0f;
    }
    return (s32)d;
}
