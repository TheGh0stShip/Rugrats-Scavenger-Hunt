typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned char u8;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

typedef struct {
    Vec3f pos;
    Vec3f pos_a;
    Vec3f pos_b;
    Vec3f velocity;
    s16 state;
    u8 pad_32[2];
    void *handle;
    s16 counter;
    u8 pad_3A[2];
} CursorMotion;

extern CursorMotion D_800544E0[2];
extern void func_8002A5EC(u32 value);

void func_8002B434(s32 state)
{
    s32 index;
    s32 other;
    s32 six;
    Vec3f from;
    Vec3f to;

    func_8002A5EC(state);
    __asm__("" : "=r"(six) : "0"(6));
    for (index = 0; index < 2; index++) {
        D_800544E0[index].state = six;
        from = D_800544E0[index].pos;
        if (state == six) {
            other = index - 1;
            if (other < 0) {
                other = 1;
            }
        } else {
            other = index + 1;
            if (other >= 2) {
                other = 0;
            }
        }
        to = D_800544E0[other].pos;
        D_800544E0[index].velocity.x = (to.x - from.x) / 6.0f;
        D_800544E0[index].velocity.y = (to.y - from.y) / 6.0f;
        D_800544E0[index].velocity.z = (to.z - from.z) / 6.0f;
    }
}
