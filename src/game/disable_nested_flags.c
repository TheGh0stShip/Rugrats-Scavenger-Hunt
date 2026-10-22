typedef unsigned char u8;
typedef unsigned int u32;

void func_8000CCE4(u8 **object, u32 flags) {
    u8 *nested = *(u8 **)(*object + 0x18);
    if (nested != 0) {
        *(u32 *)(nested + 8) &= ~flags;
    }
}
