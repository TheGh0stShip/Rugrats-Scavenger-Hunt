typedef int s32;

extern s32 D_800B9344;
extern void func_80019D44(void);
extern void func_8001E0E0(void);
extern void func_8001E178(s32 value);
extern void func_8001E204(s32 value);

void func_800309C4(void) {
    func_80019D44();
    func_8001E0E0();
    func_8001E178(0x89);
    func_8001E178(0x8B);
    func_8001E178(0x8C);
    func_8001E178(0x8D);
    func_8001E178(0x8E);
    func_8001E178(0x8F);
    func_8001E178(0x90);
    func_8001E204(0x8A);
    D_800B9344 = 1;
}
