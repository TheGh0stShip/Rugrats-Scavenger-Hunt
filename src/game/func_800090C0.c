typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

struct Arg_800090C0 {
    u8 pad_000[0x1AC];
    u32 unk1AC[4];
};
struct Obj_80057F70 {
    struct Arg_800090C0 *owner;
    u8 pad_004[0x204];
    s32 unk208;
    s32 unk20C;
    s32 unk210;
    u8 pad_214[0x14];
};
extern struct Obj_80057F70 D_80057F70;
extern void *func_80038E90(void *dst, s32 value, s32 size);

void func_800090C0(struct Arg_800090C0 *arg0) {
    s32 i;
    u32 v;
    func_80038E90(&D_80057F70, 0, sizeof(D_80057F70));
    D_80057F70.owner = arg0;
    D_80057F70.unk208 = -1;
    D_80057F70.unk210 = -1;
    for (i = 3, v = 0x8000017F; i >= 0; i--) {
        arg0->unk1AC[i] = v--;
    }
}
