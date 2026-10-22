typedef unsigned char u8;

extern u8 *D_80052F04;
extern void func_8001DEE8(void);

void func_80035864(void) {
    func_8001DEE8();
    *(unsigned int *)(D_80052F04 + 0xF4) = 0;
}
