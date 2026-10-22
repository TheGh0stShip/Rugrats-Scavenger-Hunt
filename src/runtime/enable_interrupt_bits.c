extern unsigned int func_80004CA4(void);
extern void func_80004CB0(unsigned int mask);

void func_80003A28(unsigned int bits) {
    unsigned int mask = func_80004CA4();

    func_80004CB0(mask | bits);
}
