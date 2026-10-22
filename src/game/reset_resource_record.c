typedef unsigned char u8;
typedef unsigned int u32;
extern u8 D_8005B830[];
extern u32 D_80061E00;
extern void func_80038E90(void *destination, int value, int size);
void func_8001E0B0(void) {
    func_80038E90(D_8005B830, 0, 0x13C);
    D_80061E00 = 0;
}
