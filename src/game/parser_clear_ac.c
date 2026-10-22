typedef unsigned char u8;
typedef unsigned short u16;
u8 *func_8003EEA8(void *object, u8 *input) {
    *(u16 *)((u8 *)object + 0xAC) = 0;
    return input;
}

__asm__("nop\n\tnop");
