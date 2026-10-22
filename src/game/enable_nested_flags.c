typedef unsigned char u8;
typedef unsigned int u32;

void func_8000CCB0(u8 **object, u32 flags) {
    u8 *nested = *(u8 **)(*object + 0x18);
    *(u32 *)(nested + 8) |= flags;
}
