typedef int s32;

extern s32 D_8006FB94;
extern s32 D_8006FB90;
extern int func_80010CC0(s32);
extern void func_80010FB4(s32);

void func_800227F0(void)
{
    s32 i;

    D_8006FB94 = 0;
    for (i = 0; i < 4; i++) {
        func_80010CC0(i);
        func_80010FB4(i);
    }
    D_8006FB90 = 10;
}
