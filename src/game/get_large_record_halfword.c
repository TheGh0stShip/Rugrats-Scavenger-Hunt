typedef signed short s16;
typedef unsigned char u8;

extern u8 *func_8000DD88(u8 index);

s16 func_8000E02C(u8 index, int entry) {
    register u8 *record __asm__("$2") = func_8000DD88(index);

    record += entry * 16;
    return *(s16 *)(record + 0x18);
}
