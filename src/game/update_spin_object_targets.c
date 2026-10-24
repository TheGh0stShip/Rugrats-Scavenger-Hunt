typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

struct Vec3f {
    f32 x;
    f32 y;
    f32 z;
};

struct SpinObject {
    struct Vec3f position;
    u8 pad_0C[0x18];
    struct Vec3f velocity;
    s16 state;
    u8 pad_32[0xA];
};

extern s32 D_80054AB0;
extern struct SpinObject D_80054B08[];
extern struct SpinObject *D_800C3E40;
extern const f32 D_80002FEC;
extern void func_8001BF24(s32 state);

void func_80035BD0(s32 state) {
    s32 i;
    s32 neighbor;
    struct SpinObject *object;
    struct Vec3f start;
    struct Vec3f target;

    func_8001BF24(state);
    for (i = 0; i < D_80054AB0; i++) {
        object = &D_80054B08[i];
        object->state = 6;
        D_800C3E40 = object;
        start = object->position;
        if (state == 10) {
            neighbor = i - 1;
            if (neighbor < 0) {
                neighbor = D_80054AB0 - 1;
            }
        } else {
            neighbor = i + 1;
            if (neighbor >= D_80054AB0) {
                neighbor = 0;
            }
        }
        target = D_80054B08[neighbor].position;
        D_800C3E40->velocity.x = (target.x - start.x) / D_80002FEC;
        D_800C3E40->velocity.y = (target.y - start.y) / D_80002FEC;
        D_800C3E40->velocity.z = (target.z - start.z) / D_80002FEC;
    }
}
