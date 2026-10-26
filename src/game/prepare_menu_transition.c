typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

typedef struct { f32 x, y, z; } Vec3f;

struct MenuTransition {
    Vec3f pos;
    Vec3f alternatives[2];
    Vec3f velocity;
    s16 state;
    s16 selection;
    void *handle;
    s16 counter;
    u8 pad_3A[2];
};

extern struct MenuTransition D_800544E0[2];
extern Vec3f D_80054490[];
extern s16 D_800B92EC;
extern f32 D_80002358;
extern void func_8001BF24(s32 value);
extern void func_8001DEF8(void);

void func_8002B530(s32 state) {
    s32 index;
    s32 offset;
    s32 selection;
    struct MenuTransition *entry;
    Vec3f *destination;
    Vec3f *source;

    func_8001BF24(state);
    for (index = 0; index < 2; index++) {
        entry = &D_800544E0[index];
        entry->state = 6;
        offset = D_800B92EC - 2;
        offset = (index - offset) % 2;
        selection = index != D_800B92EC;
        if (D_800B92EC == 0) {
            func_8001DEF8();
        }
        if (state == 8) {
            destination = (Vec3f *)((u8 *)D_800544E0
                + (selection * 12 + index * 0x3C) + 0xC);
            source = &D_80054490[offset];
            entry->velocity.x = (destination->x - source->x) / 6.0f;
            entry->velocity.y = (destination->y - source->y) / 6.0f;
            entry->velocity.z = (destination->z - source->z) / 6.0f;
        } else {
            destination = (Vec3f *)((u8 *)D_800544E0
                + (selection * 12 + index * 0x3C) + 0xC);
            source = &D_80054490[offset];
            entry->velocity.x = -(destination->x - source->x) / 6.0f;
            entry->velocity.y = -(destination->y - source->y) / 6.0f;
            entry->velocity.z = -(destination->z - source->z) / 6.0f;
        }
        entry->selection = selection;
    }
}
