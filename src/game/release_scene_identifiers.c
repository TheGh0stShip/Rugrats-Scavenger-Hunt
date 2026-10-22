extern void func_80019BD4(int);
extern void func_8001DEE8(void);

void func_80034300(void) {
    int id;
    for (id = 0x17C; id < 0x190; id++) func_80019BD4(id);
    func_8001DEE8();
}
