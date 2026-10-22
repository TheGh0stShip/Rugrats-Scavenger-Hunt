typedef signed short s16;
typedef unsigned char u8;

void func_8000DEEC(int row, int column, s16 value) {
    register u8 *base __asm__("$2") = (u8 *)0x80060000;

    row <<= 16;
    row >>= 9;
    column <<= 16;
    __asm__ volatile ("lw %0, -0x7E10(%0)" : "+r" (base));
    column >>= 15;
    row += (int)base;
    row += column;
    *(s16 *)(row + 0x12) = value;
}
