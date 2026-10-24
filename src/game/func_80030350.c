typedef unsigned char u8;
typedef signed int s32;

extern u8 D_80052C00;
extern s32 D_800545C0;
extern s32 D_80054348;
extern s32 D_80059D40;
extern s32 D_800B933C;
extern s32 D_80052EB8;
extern u8 D_800B9348[];
extern void func_800304A4(void);
extern void func_80030590(void);
extern void func_800305D8(void);
extern void (*D_80059DA4)(void);
extern void (*D_80059D9C)(void);
extern void (*D_80059D98)(void);
extern void func_8003028C(void);
extern void *func_8000D790(int);
extern void func_80028B58(s32, void *, s32);
extern s32 func_8001EEAC(void);
extern void func_8001ECE4(s32, s32);
extern void func_8001E718(s32);
extern void func_80012D9C(s32);
extern void func_80030200(void);

void func_80030350(void) {
    s32 arg;

    if ((!(D_80052C00 & 1) && D_800545C0 == 0) || D_80054348 == 1) {
        D_80059D40 = 9;
        func_8003028C();
    } else {
        D_80059DA4 = func_800304A4;
        D_80059D9C = func_80030590;
        D_80059D98 = func_800305D8;
    }
    switch (D_80059D40) {
        case 1:
            D_800B933C = func_8000D790(5);
            func_80028B58(D_80052EB8, D_800B9348, 0x10);
            goto end;
        case 3:
            arg = 2;
            break;
        case 4:
            arg = 4;
            break;
        case 2:
            arg = 6;
            break;
        case 5:
            arg = 7;
            break;
        case 0:
            arg = 3;
            break;
        default:
            goto end;
    }
    D_800B933C = func_8000D790(arg);
end:
    if (func_8001EEAC() != 0x1F9) {
        func_8001ECE4(0x1F9, 0x38);
        func_8001E718(0xA);
    }
    func_80012D9C(0xF);
    func_80030200();
}
