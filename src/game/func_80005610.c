typedef unsigned char u8;
typedef int s32;

typedef struct Actor {
    u8 pad_000[0x13C];
    u8 grid_x;
    u8 grid_z;
} Actor;

extern s32 D_80057D40;
extern Actor *func_800055A4(s32 index);

void func_80005610(s32 z, s32 x) {
    Actor *actor = func_800055A4(D_80057D40);

    actor->grid_z = z;
    actor->grid_x = x;
}
