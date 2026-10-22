extern int func_800039F0(void);
extern void func_80003A28(int bits);
extern int func_800046C8(int command, int first, int second, int third);

int func_80004780(void) {
    int state = func_800039F0();
    int result = func_800046C8(0x101, 0, 0, 0);

    func_80003A28(state);
    return result;
}
