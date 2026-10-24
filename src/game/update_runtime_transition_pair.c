extern unsigned char D_8000154C[];
extern void func_800139F8(void *resource, int scale, int angle);
extern void func_80016AF8(int type, unsigned char value);
extern int func_80016AA8(void *transition);

int func_80016B84(unsigned char *object)
{
    int result;

    func_800139F8(D_8000154C, 0x100, 0x5A);
    func_80016AF8(4, object[0xE]);
    result = func_80016AA8(object + 0xA);
    *(int *)(object + 4) = result;
    return func_80016AA8(object + 0xE);
}
