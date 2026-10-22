typedef signed short s16;
typedef unsigned char u8;

extern u8 D_80057F70[];

void func_8000916C(s16 row, s16 column, s16 value) {
    register u8 *base __asm__("$2") = D_80057F70;
    register u8 *address __asm__("$4");

    __asm__("" : "+r"(base));
    address = (u8 *)(row * 0x40);
    address += (unsigned int)base;
    address += column * 2;
    *(s16 *)(address + 4) = value;
}
