extern void *D_80056340;
extern void func_8004CB50(void);

void func_8004C9D0(void *state) {
    if (D_80056340 == 0) {
        D_80056340 = state;
        func_8004CB50();
    }
}
