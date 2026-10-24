extern void *func_800039F0(void);
extern void func_80003A28(void *token);
extern void func_80003EB0(unsigned int command, void *source, void *state);
extern void *func_800046A8(void *source);
extern int func_800046C8(int command, void *state, int value, int extra);

int func_800047CC(void *source, int value) {
    void *token = func_800039F0();
    void *state = func_800046A8(source);
    int result;

    func_80003EB0(0xB1FF0000, source, state);
    result = func_800046C8(0x202, state, value, 0);
    if (result & 0x8000) {
        result |= 0xFFFF0000;
    }
    func_80003A28(token);
    return result;
}
