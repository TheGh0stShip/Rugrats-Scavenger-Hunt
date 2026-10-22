extern int func_80051F80(void);
extern void func_80051FA0(unsigned int value);

void func_80051F4C(void) {
    while (func_80051F80() != 0) {
    }
    func_80051FA0(0x125);
}
