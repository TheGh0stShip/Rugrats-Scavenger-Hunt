typedef unsigned char u8;
typedef int s32;

extern u8 D_800674D0[];
extern s32 D_80053934;
extern void *D_80061DC8[];
extern s32 D_80061DA8[];
extern void func_8003C320(void *);
extern s32 func_8003BA20(void *);
extern void func_8003C090(void *, s32);
extern void func_80047E80(void);
extern void func_8003C25C(s32, s32);

void func_8001EF08(s32 arg0, s32 arg1) {
    func_8003C320(D_800674D0);
    D_80061DA8[arg0] = func_8003BA20(D_80061DC8[D_80053934]);
    func_8003C090(D_80061DA8[arg0], arg1);
    func_80047E80();
    func_8003C25C(D_80061DA8[arg0], 0x14);
}
