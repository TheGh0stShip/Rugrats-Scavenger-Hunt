/* Target: func_80015DF4  VRAM 0x80015DF4  ROM 0x169F4
 * Size: 0x300  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Cell_800581F0 {
    u8 pad_00[0x6];
    s16 unk6;
    s16 unk8;
};
struct Room_DD88 {
    s16 unk0;
    s16 unk2;
};
struct Actor_80015DF4 {
    u8 pad_00[0x8];
    s16 unk8;
    s16 unkA;
    s32 unkC;
    void *update;
    void *step;
    void *draw;
    u8 pad_1C[0xC];
    s32 unk28;
    f32 unk2C;
    u8 pad_30[0x4];
    f32 unk34;
    u8 pad_38[0xA8];
    s32 unkE0;
    s32 unkE4;
    s32 unkE8;
    u8 pad_EC[0x8];
    s32 unkF4;
    s32 unkF8;
    s32 unkFC;
    u8 pad_100[0x1C];
    s32 unk11C;
    u8 pad_120[0x4];
    s32 unk124;
    s32 unk128;
    s32 unk12C;
    u8 pad_130[0x8];
    s32 unk138;
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x12];
    s16 unk150;
    s16 unk152;
    u8 pad_154[0x44];
    s32 unk198;
    u8 pad_19C[0x4];
    u8 unk1A0[0x38];
    u8 unk1D8[0x24];
    s32 unk1FC;
};

struct Tbl_80052F18 {
    s32 unk0;
};
extern struct Tbl_80052F18 D_80052F18;
extern s16 D_8005301E[];
extern struct Cell_800581F0 *D_800581F0;
extern s8 D_800581E9;
extern s32 D_80059DA8;
extern struct Actor_80015DF4 *D_80057DA8[];
extern u8 D_80058B82[];
extern s32 D_8005B318;
extern s32 D_80057D5C;

extern void *func_8001A2B0(u32);
extern void *func_80038E90(void *, s32, s32);
extern s32 func_800197C8(s32);
extern void func_800160F4(void);
extern void func_80016138(void);
extern void func_800164EC(void);
extern void func_80016434(struct Actor_80015DF4 *);
extern void func_800162B0(struct Actor_80015DF4 *);
extern s32 func_8000E078(s32, s32);
extern struct Room_DD88 *func_8000DD88(u8);
extern void func_80005528(void *, s32);
extern void func_80007EFC(void *, void *, int, void *);
extern void func_8002E7FC(void *, s32);
extern void func_800260D0(void *, s32);
extern void func_8000E1D0(void *);
extern void func_80012CD0(void);
extern s32 func_8001BF00(void);

void func_80015DF4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    struct Actor_80015DF4 *a;
    struct Room_DD88 *room;
    struct Cell_800581F0 *c;

    a = func_8001A2B0(0x200);
    func_80038E90(a, 0, 0x200);
    a->unkE4 = arg2 + 1 + (D_80052F18.unk0 == 1);
    if (D_80052F18.unk0 == 1) {
        arg2 += 3;
    } else if (D_80052F18.unk0 == 2) {
        arg2 += 4;
    }
    a->unkE0 = arg2;
    a->unkE8 = func_800197C8(a->unkE4) + 0x2C;
    a->update = func_800160F4;
    a->step = func_80016138;
    a->draw = func_800164EC;
    a->unk8 = 3;
    a->unk28 = 0x7A00;
    a->unkA = 0;
    a->unkC = 2;
    a->unkF4 = arg0;
    a->unkF8 = arg1;
    if (D_80052F18.unk0 == 2) {
        func_80016434(a);
    } else {
        func_800162B0(a);
    }
    if (*(s32 *)&D_8005301E[1] != -1) {
        c = D_800581F0;
        c->unk6 = D_8005301E[0];
        c->unk8 = D_8005301E[2];
        D_800581E9 = func_8000E078(c->unk6, D_8005301E[2]);
        room = func_8000DD88((u8)D_800581E9);
        a->unk13C = ((u8 *)D_800581F0)[9];
        a->unk13D = ((u8 *)D_800581F0)[7];
        a->unk2C = (a->unk13C - room->unk2) * 600 + 300;
        a->unk34 = (a->unk13D - room->unk0) * 600 + 300;
        func_80005528(a, D_800581E9);
        a->unk124 = 0xB4;
        a->unk12C = 0xB4;
        a->unk128 = 0xB4;
        a->unk152 = a->unk150;
    }
    a->unkFC = 0;
    a->unk138 = -1;
    func_80007EFC(a, D_80059DA8, 4, 0);
    D_80057DA8[arg0] = a;
    if (D_80052F18.unk0 == 2) {
        func_8002E7FC(a->unk1D8, a->unkF8);
    }
    func_800260D0(a->unk1A0, arg0);
    if (arg3 == 1) {
        a->unk11C = -1;
        a->unk198 = -1;
    } else {
        a->unk11C = D_80058B82[arg3];
    }
    a->unk1FC = 0;
    func_8000E1D0(a);
    func_80012CD0();
    if (arg0 == 0 && func_8001BF00() == 0x30 && a->unk11C < 0) {
        D_8005B318 = 200;
    }
    D_80057D5C++;
}
