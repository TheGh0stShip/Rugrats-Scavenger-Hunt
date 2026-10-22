typedef signed short s16;
typedef unsigned char u8;

int func_800301C8(u8 *object, int index) {
    register s16 *entry __asm__("$5") = (s16 *)(index * 2 + (unsigned int)object);

    if (entry[0xF4] >= entry[0xF8]) {
        return 1;
    }
    return entry[0xF8] == -1;
}
