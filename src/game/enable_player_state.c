typedef unsigned char u8;
typedef unsigned int u32;

extern void func_8001BF24(int);
extern void func_80012D04(int, int);
extern u8 *func_80005544(void);

void func_800118F0(void) {
    u8 *object;
    func_8001BF24(15);
    func_80012D04(10, 1);
    object = func_80005544();
    *(u32 *)(object + 0x118) |= 0x200000;
}
