typedef unsigned int u32;
typedef int s32;

struct State_800581C0 {
    s32 pad_00[5];
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    u32 unk24;
};

extern struct State_800581C0 D_800581C0;
extern void func_8002B044(s32 a0, s32 a1, s32 a2);

void func_80027880(void) {
    u32 i;

    if (D_800581C0.unk14 != 0) {
        for (i = 0; i < D_800581C0.unk24; i++) {
            func_8002B044(D_800581C0.unk20, D_800581C0.unk1C + i, D_800581C0.unk18);
        }
    }
}
