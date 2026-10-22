typedef unsigned int u32;

extern u32 *D_800563C0;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

u32 func_80052160(void) {
    u32 status = func_80048900();
    u32 value = D_800563C0[1];

    func_80048920(status);
    return value;
}
