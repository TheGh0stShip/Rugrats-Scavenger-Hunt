typedef unsigned int u32;

extern u32 D_80057D40;
extern u32 D_80057DA8[];

u32 func_80005544(void) {
    u32 index = D_80057D40;
    u32 *slots = D_80057DA8;

    return slots[index];
}
