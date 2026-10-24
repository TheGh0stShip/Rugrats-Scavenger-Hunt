typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Sub_80031C2C {
    s32 unk0;
    u8 pad_04[0x10];
};
struct Rec_80031C2C {
    u8 pad_00[0x1C];
    struct Sub_80031C2C sub[255];
};
struct State_800C33C0 {
    s16 unk0;
    s16 unk2;
    u8 pad_04[0x4];
    s32 unk8;
    s32 unkC;
    s32 unk10;
};
extern struct Rec_80031C2C D_800B9378[];
extern struct State_800C33C0 D_800C33C0;
extern s32 D_80052EE4;
extern s32 D_8005818C;

void func_80031C2C(s32 idx) {
    D_800B9378[idx].sub[D_800C33C0.unk10].unk0 = 0;
    D_80052EE4 = 0;
    D_800B9378[idx].sub[D_800C33C0.unkC].unk0 = 0;
    D_800C33C0.unk10 = 0;
    D_800C33C0.unkC = 0;
    D_800C33C0.unk0 = 0;
    D_800C33C0.unk2 = 0;
    D_800C33C0.unk8 = 0;
    D_8005818C = 0;
}
