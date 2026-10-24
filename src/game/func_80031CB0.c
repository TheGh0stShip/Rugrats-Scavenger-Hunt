/* Target: func_80031CB0  VRAM 0x80031CB0  ROM 0x328B0
 * Size: 0xD0  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_80031CB0 {
    u8 pad_00[0x2C];
    f32 unk2C;
    u8 pad_30[0x4];
    f32 unk34;
};
struct State_800C33C0 {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    u8 pad_06[0x6];
    s32 unkC;
    s32 unk10;
};
struct Cam_800C33D8 {
    u8 pad_00[0xC];
    f32 unkC;
    u8 pad_10[0x4];
    f32 unk14;
};
struct Cfg_80057F70 {
    u8 pad_00[0x204];
    s32 unk204;
    u8 pad_208[0x14];
    s32 unk21C;
};
extern s8 D_800581E9;
extern struct Obj_80031CB0 *D_800B9360;
extern s32 D_80052EE4;
extern struct State_800C33C0 D_800C33C0;
extern struct Cam_800C33D8 D_800C33D8;
extern struct Cfg_80057F70 D_80057F70;
struct Big_800B9378 {
    u8 pad_0000[0xA044];
    s32 unkA044;
};

extern struct Big_800B9378 D_800B9378;
extern s32 func_8000E008(s32);
extern void func_8001A5C0(void);
extern struct Obj_80031CB0 *func_80005544(void);
extern void func_8003187C(s32);
extern void func_80031710(void);

void func_80031CB0(void) {
    struct Obj_80031CB0 *obj;

    if (func_8000E008(D_800581E9) == 0) {
        func_8001A5C0();
        return;
    }
    obj = func_80005544();
    D_800B9360 = obj;
    D_80052EE4 = 1;
    D_800C33D8.unkC = obj->unk2C;
    D_800C33D8.unk14 = obj->unk34;
    D_800C33C0.unkC = -1;
    D_800C33C0.unk10 = -1;
    D_800C33C0.unk0 = 0;
    D_800C33C0.unk2 = 0;
    D_800C33C0.unk4 = 0;
    if (D_80057F70.unk21C != 0) {
        s32 v = D_80057F70.unk204;
        D_800B9378.unkA044 = v;
        func_8003187C(v);
    } else {
        func_80031710();
    }
}
