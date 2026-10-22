typedef unsigned char u8;
typedef unsigned int u32;
extern u8 *func_80005544(void);
extern u8 *func_800197C8(int);
extern void func_80011BE0(void *, int, int);

void func_80011D58(int value) {
    u8 *player = func_80005544();
    if ((*(u32 *)(player + 0x118) & 0x100000) == 0) {
        u8 *resource = func_800197C8(0xEF);
        func_80011BE0(resource + 0x2C, 0, value);
    }
}
