typedef signed short s16;
typedef unsigned char u8;

s16 func_8000DF14(int row, int column) {
    register u8 *base __asm__("$2") = (u8 *)0x80060000;

    row <<= 16;
    row >>= 9;
    column <<= 16;
    __asm__ volatile ("lw %0, -0x7E10(%0)" : "+r" (base));
    column >>= 15;
    row += (int)base;
    row += column;
    return *(s16 *)(row + 0x12);
}
