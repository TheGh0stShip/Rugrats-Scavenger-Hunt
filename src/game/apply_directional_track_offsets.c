typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef float f32;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} RuntimeVector;

typedef struct {
    s16 *values;
    u8 reserved[0xA];
    u16 flags;
} DirectionalTrack;

typedef struct {
    u8 reserved[0xE];
    s16 frame;
} DirectionalTrackOwner;

extern DirectionalTrack *func_8000C490(DirectionalTrackOwner *owner);

void func_80011930(DirectionalTrackOwner *owner, RuntimeVector *vector,
                   s32 reverse_z)
{
    DirectionalTrack *track = func_8000C490(owner);
    s16 *values;

    if (track == 0) {
        return;
    }
    if (!(track->flags & 0x70)) {
        return;
    }
    values = track->values + owner->frame * (track->flags & 3);
    if (track->flags & 0x10) {
        vector->x += *values++;
    }
    if (track->flags & 0x20) {
        vector->y += *values++;
    }
    if (track->flags & 0x40) {
        if (!reverse_z) {
            vector->z += *values;
        } else {
            vector->z -= *values;
        }
    }
}
