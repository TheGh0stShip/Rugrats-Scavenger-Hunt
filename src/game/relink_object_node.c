typedef unsigned char u8;

extern u8 *D_80055AF4;
extern void func_8004CA4C(void *);
extern void func_8004CA2C(void *, void *);

void func_80040C4C(void *node) {
    func_8004CA4C(node);
    func_8004CA2C(node, D_80055AF4 + 0x14);
}
