typedef unsigned char u8;

extern u8 D_80058C00[];
extern void func_80038E90(void *dst, int value, unsigned int size);

void func_800125E0(void) {
    func_80038E90(D_80058C00, 0, 16);
}
