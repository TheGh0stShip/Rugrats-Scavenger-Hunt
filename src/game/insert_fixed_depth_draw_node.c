/* func_8000BDFC  VRAM 0x8000BDFC  ROM 0xC9FC  Size: 0x238
 * Insert a draw node at fixed depth 100 into the third sorted list.
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
extern s32 D_80052AA4;
extern s32 D_80052A9C;
extern DrawNode *D_800581B8;
extern void func_8000C1A4(struct Ctrl_DL *ctrl, void **outBuf, void **prevBuf);

DrawNode *func_8000BDFC(struct Ctrl_DL *ctrl, Vec3f pos, s32 unused, s32 a5, u32 mask) {
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
    cur = D_800581B8;
    n = D_80052AA4;
    node = &cur[n];
    node->depth = 100.0f;
    node->vtx = buf;
    node->unk10 = ctrl->model->unk08;
    node->unk14 = a5;
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
                D_80052A9C = D_80052AA4;
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
    D_80052AA4++;
    node->unk34 = ctrl->model->unk58;
    return node;
}
