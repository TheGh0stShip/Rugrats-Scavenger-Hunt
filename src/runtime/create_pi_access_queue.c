typedef unsigned int u32;

extern u32 D_80057240;
extern unsigned char D_800C6BA0[];
extern unsigned char D_800C6BB0[];
extern void func_80047EB0(void *queue, void *buffer, int count);
extern int func_80048F00(void *queue, void *message, int flags);

void func_80050EB0(void) {
    void *queue = D_800C6BB0;

    D_80057240 = 1;
    func_80047EB0(queue, D_800C6BA0, 1);
    func_80048F00(queue, 0, 0);
}
