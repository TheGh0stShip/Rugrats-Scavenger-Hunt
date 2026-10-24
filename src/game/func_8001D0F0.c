typedef short s16;
typedef int s32;

extern s32 D_80058178;
extern s16 D_800535B2;
extern void *func_80032870(s32);
extern void func_8003295C(void *, s32, s32);

void func_8001D0F0(void)
{
    if (D_80058178 >= 0) {
        func_8003295C(func_80032870(0x10), 0x100, D_800535B2);
    }
}
