typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3f;

struct SpinMotionObject {
    Vec3f position;
    Vec3f targets[2];
    Vec3f step;
    s16 state;
    u8 pad_32[0xA];
};

extern s32 D_80054AB0;
extern Vec3f D_80054AE4[];
extern struct SpinMotionObject D_80054B08[];
extern struct SpinMotionObject *D_800C3E40;
extern s16 D_800C3E44;
extern const f32 D_80002FE8;
extern void func_8001BF24(s32 state);

void func_80035A2C(s32 state) {
    s32 i;
    s32 slot;
    s32 side;
    s32 offset;
    struct SpinMotionObject *object;

    func_8001BF24(state);
    offset = 0;
    for (i = 0; i < D_80054AB0; i++) {
        side = i != D_800C3E44;
        slot = (i + D_80054AB0 - D_800C3E44) % D_80054AB0;
        object = &D_80054B08[i];
        D_800C3E40 = object;
        object->state = 6;
        if (state == 12) {
            struct SpinMotionObject *point = (struct SpinMotionObject *)((u8 *)D_80054B08 + offset + side * 12);
            object->step.x = (point->targets[0].x - D_80054AE4[slot].x) / D_80002FE8;
            object->step.y = (point->targets[0].y - D_80054AE4[slot].y) / D_80002FE8;
            object->step.z = (point->targets[0].z - D_80054AE4[slot].z) / D_80002FE8;
        } else {
            struct SpinMotionObject *point = (struct SpinMotionObject *)((u8 *)D_80054B08 + offset + side * 12);
            object->step.x = -(point->targets[0].x - D_80054AE4[slot].x) / D_80002FE8;
            object->step.y = -(point->targets[0].y - D_80054AE4[slot].y) / D_80002FE8;
            object->step.z = -(point->targets[0].z - D_80054AE4[slot].z) / D_80002FE8;
        }
        offset += sizeof(struct SpinMotionObject);
    }
}
