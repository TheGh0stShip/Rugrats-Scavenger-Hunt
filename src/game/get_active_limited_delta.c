typedef signed char s8;
typedef unsigned char u8;

extern u8 *D_8005B214;

int func_8001B5BC(void) {
    int first = 30 - *(s8 *)(D_8005B214 + 0x1A6);
    int second = *(s8 *)(D_8005B214 + 0x1A7) - 20;
    return second < first ? second : first;
}
