typedef unsigned char u8;
typedef unsigned int u32;
extern u8 D_8005B830[];
void func_8001E28C(u32 offset, u32 upper) {
    u8 *base = D_8005B830;
    u32 lower = *(u32 *)(base + 0xAC);
    upper = *(u32 *)(base + 0x138);
    lower += offset;
    upper += offset;
    *(u32 *)(base + 0xAC) = lower;
    *(u32 *)(base + 0x138) = upper;
}
