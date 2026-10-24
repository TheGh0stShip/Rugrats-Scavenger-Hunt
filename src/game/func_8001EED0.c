typedef int s32;

extern void *D_8005392C;
extern void func_8001ECE4(void *, s32);

void *func_8001EED0(void) {
    void *p;
    func_8001ECE4(D_8005392C, 0x38);
    p = D_8005392C;
    D_8005392C = 0;
    return p;
}

