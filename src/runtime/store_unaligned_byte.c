typedef unsigned char u8;
typedef unsigned int u32;

void func_80003C70(u8 *address, u8 value) {
    u32 shift = (~(u32)address & 3) * 8;
    u32 *word = (u32 *)((u32)address & ~3U);
    u32 mask = ~(0xFFU << shift);

    *word = (*word & mask) | ((u32)value << shift);
}
