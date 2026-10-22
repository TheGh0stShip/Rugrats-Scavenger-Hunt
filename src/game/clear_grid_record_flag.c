typedef unsigned short u16;
typedef unsigned char u8;

void func_8000DE4C(int row, int column) {
    register u8 *base __asm__("$2") = (u8 *)0x80060000;
    u16 *entry;

    row <<= 16;
    row >>= 9;
    column <<= 16;
    __asm__ volatile ("lw %0, -0x7E10(%0)" : "+r" (base));
    column >>= 15;
    row += (int)base;
    row += 0x12;
    column += row;
    entry = (u16 *)column;
    *entry &= 0x7FFF;
}
