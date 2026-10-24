typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
typedef int s32;
typedef float f32;

struct Cell_800581F0 {
    u8 pad_00[6];
    s16 unk6;
    s16 unk8;
};

struct Room_DD88 {
    s16 unk0;
    s16 unk2;
};

struct Obj_800162B0 {
    u8 pad_00[0x2C];
    f32 unk2C;
    u8 pad_30[4];
    f32 unk34;
    u8 pad_38[0xEC];
    s32 unk124;
    s32 unk128;
    s32 unk12C;
    u8 pad_130[0xC];
    u8 unk13C;
    u8 unk13D;
    u8 pad_13E[0x12];
    s16 unk150;
    s16 unk152;
};

extern struct Cell_800581F0 *D_800581F0;
extern s8 D_800581E9;
extern s32 func_8003A840(void);
extern s32 func_8000DF14(s16, s16);
extern s32 func_8000E078(s32, s32);
extern s32 func_8000E108(void);
extern struct Room_DD88 *func_8000DD88(u8);
extern void func_80005528(void *, s32);

void func_800162B0(struct Obj_800162B0 *arg0)
{
    s32 x;
    s32 y;
    struct Room_DD88 *room;
    struct Cell_800581F0 *cell;

    do {
        do {
            x = func_8003A840() % 64;
            y = func_8003A840() % 64;
        } while (!(func_8000DF14(x, y) & 0x8000));
    } while (func_8000E078(x, y) == func_8000E108());

    cell = D_800581F0;
    cell->unk6 = x;
    cell->unk8 = y;
    D_800581E9 = func_8000E078(cell->unk6, (s16)y);
    room = func_8000DD88((u8)D_800581E9);
    arg0->unk13C = ((u8 *)D_800581F0)[9];
    arg0->unk13D = ((u8 *)D_800581F0)[7];
    arg0->unk2C = (arg0->unk13C - room->unk2) * 600 + 300;
    arg0->unk34 = (arg0->unk13D - room->unk0) * 600 + 300;
    func_80005528(arg0, D_800581E9);
    arg0->unk124 = 0xB4;
    arg0->unk12C = 0xB4;
    arg0->unk128 = 0xB4;
    arg0->unk152 = arg0->unk150;
}
