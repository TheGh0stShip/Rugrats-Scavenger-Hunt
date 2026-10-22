typedef unsigned char u8;
typedef unsigned int u32;
extern u8 *D_80053634;
int func_8002A5C8(void) {
    if (D_80053634 == 0) return -1;
    return *(u32 *)(D_80053634 + 0x28);
}
