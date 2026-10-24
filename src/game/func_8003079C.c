typedef signed int s32;

extern void func_80030840(void);
extern void func_80030914(void);
extern void func_8003099C(void);
extern void (*D_80059DA4)(void);
extern void (*D_80059D9C)(void);
extern void (*D_80059D98)(void);
extern s32 D_8005463C;
extern s32 D_8005435C;
extern s32 D_800B9344;
extern void func_8001BDD0(void);
extern void func_8001906C(void);
extern void func_8001BED8(s32);
extern void func_80012D9C(s32);
extern void func_8001E718(s32);
extern void func_8001ECE4(s32, s32);

void func_8003079C(void) {
    D_80059DA4 = func_80030840;
    D_80059D9C = func_80030914;
    D_80059D98 = func_8003099C;
    func_8001BDD0();
    func_8001906C();
    func_8001BED8(1);
    if (D_8005463C != 0) {
        D_800B9344 = 0;
    } else {
        D_800B9344 = 3;
    }
    D_8005435C = 0;
    func_80012D9C(0xF);
    func_8001E718(0xF);
    func_8001ECE4(0x205, 0x18);
}
