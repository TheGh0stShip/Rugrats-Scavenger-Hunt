typedef unsigned char u8;
typedef unsigned int u32;

extern u8 *D_80053634;

int func_8001BF3C(void) {
    u8 *active = D_80053634;
    int result;
    if (active == 0) {
        result = -1;
    } else {
        result = *(u32 *)(active + 0x2C);
    }
    return result;
}
