typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef signed int s32;

struct Item_80031328 {
    u8 pad_00[0x8];
    s16 unk8;
};

struct Slot_80031328 {
    struct Item_80031328 *item;
    s32 idx;
};

struct Rec_80031328 {
    u8 pad_00[0x210];
};

extern s16 D_800B9364;
extern struct Slot_80031328 *D_800B936C;
extern s32 D_80054810;
extern struct Rec_80031328 *D_800581F0;
extern s32 func_8003A840(void);

void func_80031328(s16 val) {
    s32 r;
    struct Slot_80031328 *slot;

    if (--D_800B9364 >= 0) {
        do {
            r = func_8003A840() % D_80054810;
        } while (D_800B936C[r].item->unk8 >= 0);
        slot = &D_800B936C[r];
        slot->item->unk8 = val;
        r = slot->idx;
        (*(u16 *)((u8 *)&D_800581F0[r] + 0x2020))++;
    }
}
