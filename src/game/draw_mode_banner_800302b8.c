extern int D_80054348;
extern void func_8001F588(int); extern void func_80013724(void);
extern void func_8001EFEC(int, int); extern void func_800139F8(void *, int, int);
extern char D_80002AA0[]; extern char D_80002AB0[];
void func_800302B8(void) {
    if (D_80054348 == 0) {
        func_8001F588(0);
        func_80013724();
        func_800139F8(D_80002AA0, 0x100, 0x6E);
    } else {
        func_8001EFEC(0, 0);
        func_8001F588(0);
        func_80013724();
        func_800139F8(D_80002AB0, 0x100, 0x6E);
    }
}
