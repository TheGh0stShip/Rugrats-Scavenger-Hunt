typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef float f32;

typedef struct {
    s16 *values;
    u8 reserved[0xA];
    u16 flags;
} PositionTrack;

typedef struct {
    u8 reserved[0xE];
    s16 frame;
} TrackOwner;

extern PositionTrack *func_8000C490(TrackOwner *owner);

void func_8000C3AC(TrackOwner *owner, f32 *position)
{
    PositionTrack *track;
    s16 *values;

    track = func_8000C490(owner);
    if (track != 0 && (track->flags & 0x70)) {
        values = track->values + owner->frame * (track->flags & 3);
        if (track->flags & 0x10) {
            position[0] += *values++;
        }
        if (track->flags & 0x20) {
            position[1] += *values++;
        }
        if (track->flags & 0x40) {
            position[2] += *values;
        }
    }
}
