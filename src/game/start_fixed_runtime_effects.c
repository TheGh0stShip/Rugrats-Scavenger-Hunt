extern unsigned char D_800021FC[];
extern unsigned char D_800021A4[];
extern unsigned char D_800021A8[];
extern void func_800139F8(void *, int, int);
extern void func_8001380C(int, void *, int, int);

void func_80029D68(void) {
    volatile char scratch[40];

    func_800139F8(D_800021FC, 0x100, 0xA0);
    func_8001380C(0x38, D_800021A4, 100, 200);
    func_8001380C(0x37, D_800021A8, 300, 200);
}
