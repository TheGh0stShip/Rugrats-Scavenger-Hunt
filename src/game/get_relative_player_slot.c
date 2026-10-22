typedef unsigned int u32;

extern u32 D_80057D5C;
u32 func_800055A4(u32 index);

u32 func_800055C0(u32 offset) {
    return func_800055A4(D_80057D5C + offset);
}
