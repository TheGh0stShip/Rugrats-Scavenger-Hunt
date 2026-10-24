typedef unsigned char u8;
typedef signed short s16;
typedef signed int s32;

struct Player_800255AC {
    u8 pad_00[0x144];
    s32 unk144;
    u8 pad_148[0x150 - 0x148];
    s16 unk150;
};

extern s32 D_80057D5C;
extern struct Player_800255AC *func_800055A4(s32);
extern s32 func_80006DB0(s32, s32);
extern void func_80006534(struct Player_800255AC *, s32, s32);

void func_800255AC(struct Player_800255AC *self) {
    struct Player_800255AC *other;
    s32 i;
    s32 j;
    s32 target;
    s32 found;

    for (i = 0; i < D_80057D5C; i++) {
        other = func_800055A4(i);
        if (other != self && other->unk150 == self->unk150 && other->unk144 == self->unk144) {
            goto found_other;
        }
        other = 0;
    }
found_other:
    if (other == 0) {
        return;
    }
    for (i = 1; i < 4; i++) {
        target = (self->unk144 + i) % func_80006DB0(self->unk150, 0);
        j = 0;
        found = j;
        for (; j < D_80057D5C; j++) {
            if (func_800055A4(j)->unk150 == self->unk150 && func_800055A4(j)->unk144 == target) {
                found = 1;
                break;
            }
        }
        if (!found) {
            func_80006534(other, other->unk150, target);
            return;
        }
    }
}
