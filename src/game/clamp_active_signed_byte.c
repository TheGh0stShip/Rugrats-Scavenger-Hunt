typedef signed char s8;
typedef unsigned char u8;

extern u8 *D_8005B214;

void func_8001C2AC(void) {
    if (*(s8 *)(D_8005B214 + 0x1A6) < 0) {
        D_8005B214[0x1A6] = 0;
    }
}
