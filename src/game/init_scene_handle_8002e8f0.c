extern int D_800B9318; extern int D_800545E0;
extern int func_8000D050(int); extern void func_8001DEF8(void);
void func_8002E8F0(void) {
    int *p = &D_800B9318;
    *p = func_8000D050(9);
    func_8001DEF8();
    D_800545E0 = 1;
    __asm__ __volatile__("");
}
