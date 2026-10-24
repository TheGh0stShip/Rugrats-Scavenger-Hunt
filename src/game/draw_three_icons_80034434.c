extern short D_800549F0[][2];
extern void func_80033144(int, int, int, int);
extern int func_80032870(int);
extern void func_80032A20(int, int, int);

void func_80034434(void) {
    int i;
    func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
    for (i = 0; i < 3; i++) {
        func_80032A20(func_80032870(i + 0x3C), D_800549F0[i][0], D_800549F0[i][1]);
    }
}
