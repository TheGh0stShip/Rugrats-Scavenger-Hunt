typedef unsigned char u8;
typedef unsigned int u32;

u8 *func_8000FAE8(u8 *base) {
    *(u8 **)(base + 8) = base + *(u32 *)(base + 8);
    *(u8 **)(base + 12) = base + *(u32 *)(base + 12);
    return base;
}
