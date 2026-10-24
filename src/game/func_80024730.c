typedef int s32;

extern s32 D_8006FB80;
extern s32 func_800206D0(void);
extern void func_80005500(s32, s32);
extern void func_8000550C(s32, s32);
extern void func_80005514(s32, s32);
extern void func_8000FDAC(s32, s32);

void func_80024730(s32 arg0) {
    D_8006FB80 = arg0;
    func_80005500(8, arg0);
    func_8000550C(5, arg0);
    func_80005500(8, func_800206D0());
    func_8000550C(5, func_800206D0());
    func_80005514(arg0, 2);
    func_80005514(func_800206D0(), 2);
    func_8000FDAC(7, arg0);
}
