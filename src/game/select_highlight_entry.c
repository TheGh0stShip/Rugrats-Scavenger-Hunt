/* func_8003187C: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Entry_8003187C {
    s16 unk0;
    s16 unk2;
    u8 pad_04[0x4];
    s16 unk8;
    u8 pad_0A[0x2];
    s32 unkC;
};
struct Obj_8003187C {
    u8 pad_00[0x10];
    struct Entry_8003187C entries[1];
};
struct State_800C33C0 {
    s16 unk0;
    s16 unk2;
    u8 pad_04[0x4];
    s32 unk8;
    s32 unkC;
    s32 unk10;
};
struct Cam_800C33D8 {
    f32 unk0;
    s32 unk4;
    f32 unk8;
    u8 pad_0C[0x20];
    s32 unk2C;
};
extern u8 D_800581E9;
extern struct State_800C33C0 D_800C33C0;
extern struct Cam_800C33D8 D_800C33D8;
extern struct Obj_8003187C *func_8000DD88(s32);

void func_8003187C(s32 sel) {
    struct Obj_8003187C *obj;
    struct Entry_8003187C *e;
    s32 prev;

    obj = func_8000DD88(D_800581E9);
    if (D_800C33C0.unkC == -1) {
        D_800C33C0.unk10 = -1;
        D_800C33C0.unkC = sel;
    } else {
        if (D_800C33C0.unk10 >= 0) {
            obj->entries[D_800C33C0.unk10].unkC = 0x80000;
        }
        prev = D_800C33C0.unkC;
        D_800C33C0.unkC = sel;
        D_800C33C0.unk10 = prev;
    }
    if (D_800C33C0.unk10 >= 0) {
        obj->entries[D_800C33C0.unk10].unkC = 0x20000;
    }
    obj->entries[D_800C33C0.unkC].unkC = 0x10000;
    D_800C33C0.unk0 = 0x4B;
    D_800C33C0.unk2 = 0xFF;
    D_800C33C0.unk8 = 5;
    e = (struct Entry_8003187C *)((u8 *)obj + D_800C33C0.unkC * 16);
    D_800C33D8.unk0 = e[1].unk2 * 600 + 300;
    D_800C33D8.unk4 = 0;
    D_800C33D8.unk2C = 1;
    D_800C33D8.unk8 = e[1].unk0 * 600 + 300;
}
