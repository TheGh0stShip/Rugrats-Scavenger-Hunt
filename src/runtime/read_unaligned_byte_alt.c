typedef unsigned char u8;
typedef unsigned int u32;

extern u32 func_80003A90(u32 *address);

u8 func_80003B54(u8 *address) {
    u32 shift = (~(u32)address & 3) * 8;

    return (func_80003A90((u32 *)((u32)address & ~3U)) >> shift) & 0xFF;
}
