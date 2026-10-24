extern int D_8005A990[];
extern void func_80019C50(void);
extern int func_80013238(int);

int func_80032870(int i) {
    func_80019C50();
    if (D_8005A990[i] == 0) {
        D_8005A990[i] = func_80013238(i);
    }
    return D_8005A990[i];
}
