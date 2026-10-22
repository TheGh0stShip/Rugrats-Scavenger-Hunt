extern void *D_80056340;
extern void func_8004D1E0(void);

void func_8004C9FC(void) {
    if (D_80056340 != 0) {
        func_8004D1E0();
        D_80056340 = 0;
    }
}
