typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Actor_80020D68 {
    u8 pad_00[0xFC];
    void *unkFC;
    u8 pad_100[0x150 - 0x100];
    s16 unk150;
};

extern s32 D_80057D94;
extern s8 D_800581E9;
extern void func_800212D0(struct Actor_80020D68 *);

void func_80020D68(struct Actor_80020D68 *a)
{
    if (D_80057D94 != 1 && a->unk150 == D_800581E9 && a->unkFC != 0) {
        func_800212D0(a);
    }
}

