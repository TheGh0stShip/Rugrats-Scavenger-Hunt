extern void func_8001BED8(int);
extern void func_800069C8(int, int, int);
extern void func_80005500(int, int);
extern void func_8000FDAC(int, int);

void func_80024BDC(int value) {
    func_8001BED8(0x10);
    func_800069C8(2, value, 1);
    func_80005500(0, value);
    func_8000FDAC(10, value);
}
