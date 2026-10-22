typedef unsigned char u8;
typedef unsigned int u32;
extern u32 D_800C4000;
extern void func_800425F0(u8 value);
u8 *func_8003EDF4(void *object, u8 *input) {
    u8 value = *input++;
    if (D_800C4000 == 1) func_800425F0(value);
    return input;
}
