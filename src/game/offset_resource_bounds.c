typedef unsigned char u8;
typedef unsigned int u32;
extern u8 D_8005B830[];
void func_8001E28C(u32 offset) {
    *(u32 *)(D_8005B830 + 0xAC) += offset;
    *(u32 *)(D_8005B830 + 0x138) += offset;
}
