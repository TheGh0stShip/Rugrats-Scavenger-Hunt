extern void *D_8005A990[];
extern void func_80019C50(void);
extern void *func_80013238(int);

void *func_8000D050(int index) {
    func_80019C50();
    if (D_8005A990[index] == 0) {
        D_8005A990[index] = func_80013238(index);
    }
    return D_8005A990[index];
}
