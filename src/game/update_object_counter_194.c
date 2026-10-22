typedef signed short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

void func_8002E17C(u8 *object) {
    if (*(s16 *)(object + 0x194) > 0) {
        *(u16 *)(object + 0x194) = *(u16 *)(object + 0x194) - 1;
    } else {
        *(u32 *)(object + 0x18C) |= 0x10;
    }
    __asm__("" : : "r"(object));
}
