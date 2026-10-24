extern void *D_80059DA8; extern struct G { char pad[0x30]; int x; } *D_800B933C;
extern void func_80008240(void *); extern void func_8000D768(int);
void func_800305D8(void) {
    func_80008240(D_80059DA8);
    if (D_800B933C != 0) {
        func_8000D768(D_800B933C->x);
        D_800B933C = 0;
    } else {
        D_800B933C = 0;
    }
}
