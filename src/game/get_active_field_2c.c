typedef unsigned char u8;
typedef unsigned int u32;
extern u8 *D_80053634;
int func_8002A604(void) {
    register u8 *active __asm__("$2") = D_80053634;
    int result;
    if (active == 0) {
        result = -1;
    } else {
        result = *(u32 *)(active + 0x2C);
    }
    return result;
}
