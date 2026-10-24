/* func_8000BB80  VRAM 0x8000BB80  ROM 0xC780  Size: 0x27C
 * Insert a draw node into the second list sorted by camera distance.
 * SN GCC 2.8 -O2 with reviewed scheduling normalization. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { f32 x, y, z; } Vec3f;

typedef struct DrawNode {
    struct DrawNode *next;  /* 0x00 */
    struct DrawNode *prev;  /* 0x04 */
    f32 depth;              /* 0x08 */
    void *vtx;              /* 0x0C */
    s32 unk10;
    s32 unk14;
    s32 unk18;
    void *owner;            /* 0x1C */
    u32 flags;              /* 0x20 */
    s32 unk24;
    s32 unk28;
    u16 unk2C;
    u16 unk2E;
    s32 unk30;
    s32 unk34;
} DrawNode; /* size 0x38 */

struct AnimSet_DL {
    u8 pad_00[8];
    u32 flags;
};

struct Model_DL {
    u16 flags;              /* 0x00 */
    u8 pad_02[6];
    s32 unk08;
    void *bufA;             /* 0x0C */
    u8 pad_10[8];
    struct AnimSet_DL *anims; /* 0x18 */
    u8 pad_1C[4];
    s32 unk20;
    u8 pad_24[0x24];
    s32 unk48;
    s32 unk4C;
    u16 unk50;
    u16 unk52;
    s32 unk54;
    s32 unk58;
};

struct Ctrl_DL {
    struct Model_DL *model;
};

extern u32 D_80052AB8;
extern s32 D_80052AB4;
extern s32 D_80052AAC;
extern DrawNode *D_800581B0;
extern void func_8000C1A4(struct Ctrl_DL *ctrl, void **outBuf, void **prevBuf);
extern s32 func_80008B80(Vec3f a, Vec3f b);
extern u8 *D_80052F04;

DrawNode *func_8000BB80(struct Ctrl_DL *ctrl, Vec3f pos, s32 unused, s32 a4, u32 mask) {
    u8 done = 0;
    void *buf;
    void *prev;
    DrawNode *cur;
    DrawNode *node;
    DrawNode *nb;
    s32 n;

    if (D_80052AB8 != 0 && !(mask & D_80052AB8)) {
        return 0;
    }
    func_8000C1A4(ctrl, &buf, &prev);
    if (ctrl->model->anims != 0 && (ctrl->model->anims->flags & 2)) {
        mask |= 0x2000000;
    }
    cur = D_800581B0;
    {
        f32 d = func_80008B80(*(Vec3f *)(D_80052F04 + 0x28), pos);
        n = D_80052AB4;
        node = &D_800581B0[n];
        node->depth = d;
    }
    node->vtx = buf;
    node->unk10 = ctrl->model->unk08;
    node->unk14 = a4;
    node->unk18 = ctrl->model->unk20;
    node->owner = ctrl;
    if (cur == node) {
        done = 1;
    }
    while (!done) {
        if (node->depth > cur->depth) {
            nb = cur->next;
            if (nb == 0) {
                done = 1;
                node->prev = cur;
                cur->next = node;
                D_80052AAC = D_80052AB4;
            } else if (node->depth <= nb->depth) {
                node->prev = cur;
                node->next = cur->next;
                cur->next->prev = node;
                done = 1;
                cur->next = node;
            } else {
                cur = nb;
            }
        } else {
            nb = cur->prev;
            if (nb == 0) {
                done = 1;
                cur->prev = node;
                node->next = cur;
            } else if (nb->depth <= node->depth) {
                node->prev = nb;
                node->next = cur;
                cur->prev->next = node;
                done = 1;
                cur->prev = node;
            } else {
                cur = nb;
            }
        }
    }
    node->flags = ctrl->model->flags | mask;
    node->unk24 = ctrl->model->unk48;
    node->unk28 = ctrl->model->unk4C;
    node->unk2C = ctrl->model->unk50;
    node->unk2E = ctrl->model->unk52;
    node->unk30 = ctrl->model->unk54;
    D_80052AB4++;
    node->unk34 = ctrl->model->unk58;
    return node;
}
