/* func_8000C1A4  VRAM 0x8000C1A4  ROM 0xCDA4  Size: 0x208
 * Select double-buffered vertex output for the current animation key.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

struct AnimSet_8000C1A4 {
    u8 pad_00[8];
    u32 flags;
};

struct Model_8000C1A4 {
    u16 flags;
    u8 pad_02[0xA];
    void *bufA;     /* 0x0C */
    void *bufB;     /* 0x10 */
    u8 pad_14[4];
    struct AnimSet_8000C1A4 *anims; /* 0x18 */
};

struct Track_8000C1A4 {
    u8 pad_00[0xC];
    u16 numKeys;    /* 0x0C */
    u8 pad_0E[6];
    u16 keys[1];    /* 0x14 */
};

struct Ctrl_8000C1A4 {
    struct Model_8000C1A4 *model;
    u8 pad_04[4];
    s16 cur;        /* 0x08 */
    u8 pad_0A[6];
    s16 frame;      /* 0x10 */
    s16 limit;      /* 0x12 */
};

extern struct Track_8000C1A4 *func_8000C490(struct Ctrl_8000C1A4 *ctrl);
extern s32 func_8000CD24(struct Model_8000C1A4 *m, s32 idx, s32 arg);
extern s16 func_8000B858(u16 key, u16 *list, u16 count, s16 *lastBelow);
extern void func_8000B380(struct Model_8000C1A4 *m, struct Track_8000C1A4 *trk, u16 key, void *buf, s32 delta);
extern void func_8000B71C(struct Model_8000C1A4 *m, void *out, void *in);
extern void func_80047E80(void);

void func_8000C1A4(struct Ctrl_8000C1A4 *ctrl, void **outBuf, void **prevBuf) {
    struct Model_8000C1A4 *m = ctrl->model;
    struct Track_8000C1A4 *trk;
    u16 *keys;
    s32 delta;
    s16 key;
    s16 below;

    trk = func_8000C490(ctrl);
    delta = 0;
    keys = trk->keys;
    if ((m->flags & 0x2000) && trk != 0) {
    if (func_8000CD24(m, ctrl->cur, 1) && (m->anims->flags & 1)) {
        if (m->flags & 0x8000) {
            *outBuf = m->bufB;
            *prevBuf = m->bufA;
        } else {
            *outBuf = m->bufA;
            *prevBuf = m->bufB;
        }
    } else {
        if (m->flags & 0x8000) {
            *outBuf = m->bufA;
            *prevBuf = m->bufB;
            m->flags &= 0x7FFF;
        } else {
            *outBuf = m->bufB;
            *prevBuf = m->bufA;
            m->flags |= 0x8000;
        }
        key = func_8000B858(ctrl->frame, keys, trk->numKeys, &below);
        if ((m->anims->flags & 8) && key == -1) {
            if (ctrl->limit < keys[(u16)below]) {
                key = below;
                delta = ctrl->frame - keys[below];
            }
        }
        if (key >= 0) {
            m->flags |= 0x4000;
            func_8000B380(m, trk, key, *outBuf, delta);
        } else {
            m->flags &= ~0x4000;
            func_8000B71C(m, *outBuf, *prevBuf);
        }
    }
    func_80047E80();
    } else {
        *outBuf = m->bufA;
    }
}
