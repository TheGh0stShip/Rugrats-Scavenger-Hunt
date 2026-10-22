typedef void *Ptr;

extern Ptr D_80055AF0;
extern Ptr D_80055AF4;
extern void func_80040440(void);

void func_8003EF54(void) {
    if (D_80055AF0 != 0) {
        func_80040440();
        D_80055AF0 = 0;
        D_80055AF4 = 0;
    }
}
