extern int D_80053934;
extern void *D_80061DC8[];
extern void func_8001313C(int target, void *resource);

int func_8001ED90(int target) {
    int index = D_80053934;
    void *resource = D_80061DC8[index];

    func_8001313C(target, resource);
    return 1;
}
