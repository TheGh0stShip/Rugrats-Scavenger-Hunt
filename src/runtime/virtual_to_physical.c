typedef unsigned int u32;

extern u32 func_80049630(void *address);

u32 func_800495D0(void *address)
{
    if (((u32)address >= 0x80000000) && ((u32)address < 0xA0000000)) {
        return (u32)address & 0x1FFFFFFF;
    } else if (((u32)address >= 0xA0000000) &&
               ((u32)address < 0xC0000000)) {
        return (u32)address & 0x1FFFFFFF;
    } else {
        return func_80049630(address);
    }
}
