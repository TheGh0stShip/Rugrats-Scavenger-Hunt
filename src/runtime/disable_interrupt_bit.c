extern unsigned int func_80004CA4(void);
extern void func_80004CB0(unsigned int mask);

int func_800039F0(void) {
    unsigned int mask = func_80004CA4();

    func_80004CB0(mask & ~1U);
    return mask & 1;
}
