typedef unsigned char u8;
typedef int s32;

struct Res_80026310 {
    u8 pad_00[0x2C];
    s32 unk2C;
};
struct Slot_80026310 {
    s32 unk00;
    u8 pad_04[0x14];
};

extern struct Slot_80026310 D_8006FBF8[];
extern struct Res_80026310 *func_800197C8(s32 arg0);
extern void func_8000CE7C(struct Slot_80026310 *slot, s32 index);

void func_80026310(s32 arg0) {
    s32 *src;
    s32 i;
    struct Slot_80026310 *slot;

    src = &func_800197C8(arg0 + 0xF1)->unk2C;
    i = 0;
    slot = D_8006FBF8;
    for (; i < 39; i++) {
        slot->unk00 = *src;
        func_8000CE7C(slot, i);
        slot++;
    }
}
