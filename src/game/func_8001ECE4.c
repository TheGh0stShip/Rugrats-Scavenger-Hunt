typedef int s32;

extern s32 D_80053928;
extern s32 D_80053934;
extern s32 D_80061DA8[7];
extern s32 D_80061E50;
extern void func_8001F30C(void);
extern void func_8003BE38(s32, s32);
extern s32 func_8003BF04(s32);
extern void func_8001ED90(s32, s32, s32);
extern void func_8001EF08(s32, s32, s32);

void func_8001ECE4(s32 arg0, s32 arg1)
{
    s32 i;

    func_8001F30C();
    func_8003BE38(3, 0);
    while (func_8003BF04(3) > 0) {
    }
    D_80053928 = arg0;
    D_80053934 = (D_80053934 == 0);
    for (i = 6; i >= 0; i--) {
        D_80061DA8[i] = 0;
    }
    func_8001ED90(arg0, 7, 0);
    func_8001EF08(0, arg1, arg0);
    D_80061E50 = 0;
}
