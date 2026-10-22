typedef unsigned int u32;

extern u32 D_80056050;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

void func_80050AF0(u32 flags) {
    u32 status = func_80048900();

    D_80056050 &= ~flags | 0x401;
    func_80048920(status);
}
