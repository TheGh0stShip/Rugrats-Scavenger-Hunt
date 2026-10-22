extern char *D_800B933C;
extern void *func_8000D050(int identifier);
extern void func_8000D0AC(void *resource);
extern void func_80014F00(void);
extern void func_80013B14(void);

void func_80030590(void) {
    void *resource;

    if (D_800B933C != 0) {
        resource = func_8000D050(*(int *)(D_800B933C + 0x30));
        func_8000D0AC(resource);
    }
    func_80014F00();
    func_80013B14();
}
