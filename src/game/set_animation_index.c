/* func_8000C530  VRAM 0x8000C530  ROM 0xD130  Size: 0x144
 * Change the active animation index for a model controller.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct AnimEntry_8000C530 {
    u8 pad_00[0x8];
    u16 unk8;
    u8 pad_0A[0x2];
};

struct AnimSet_8000C530 {
    u8 pad_00[2];
    u16 count;
    u8 pad_04[4];
    u32 flags;
    u8 pad_0C[0xC];
    struct AnimEntry_8000C530 entries[1];
};

struct Model_8000C530 {
    u8 pad_00[0x18];
    struct AnimSet_8000C530 *anims;
};

struct Ctrl_8000C530 {
    struct Model_8000C530 *model;
    u8 pad_04[4];
    s16 cur;
    s16 next;
    u8 pad_0C[2];
    s16 frame;
    s16 length;
};

extern u16 *func_8000CE50(struct Model_8000C530 *m, u32 idx, s32 arg);

void func_8000C530(struct Ctrl_8000C530 *ctrl, u32 idx, s32 mode) {
    struct Model_8000C530 *m = ctrl->model;
    struct AnimSet_8000C530 *set = m->anims;

    if (set == 0) {
        return;
    }
    if (idx >= set->count) {
        ctrl->frame = 0;
        ctrl->cur = 0;
        ctrl->next = 0;
        return;
    }
    set->flags &= ~4;
    switch (mode) {
        case 3:
            m->anims->flags |= 4;
        case 0:
            if (idx != ctrl->cur) {
                ctrl->frame = 0;
                if (m->anims->entries[idx].unk8 != 0) {
                    ctrl->length = func_8000CE50(m, idx, 0)[1];
                } else {
                    ctrl->length = 0;
                }
                m->anims->flags &= ~1;
            }
            break;
        case 1:
            ctrl->frame = 0;
            if (m->anims->entries[idx].unk8 != 0) {
                ctrl->length = func_8000CE50(m, idx, 0)[1];
            } else {
                ctrl->length = 0;
            }
            m->anims->flags &= ~1;
            break;
        case 2:
            ctrl->next = idx;
            return;
        default:
            return;
    }
    ctrl->cur = idx;
    ctrl->next = idx;
}
