typedef unsigned int u32;

extern u32 D_800570F0;
extern unsigned char D_800C4590[];
extern unsigned char D_800C45A0[];
extern void func_80047EB0(void *queue, void *buffer, int count);
extern int func_80048F00(void *queue, void *message, int flags);

void func_8004DDD0(void) {
    void *queue = D_800C45A0;

    D_800570F0 = 1;
    func_80047EB0(queue, D_800C4590, 1);
    func_80048F00(queue, 0, 0);
}
