extern unsigned char *D_800C3FD8;
extern void func_80040750(void *owner);
extern void func_800406C0(void *owner, int value);

void func_8003D014(unsigned char *object, int index)
{
    unsigned char *owner;

    if (object[0xC9] != 0) {
        func_80040750(D_800C3FD8 + index * 28);
    }
    object[0xC9] = 1;
    owner = D_800C3FD8 + index * 28;
    func_800406C0(owner, *(int *)(object + 8));
    *(int *)(object + 8) = 0;
}
