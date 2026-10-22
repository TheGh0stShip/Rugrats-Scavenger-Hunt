typedef unsigned int u32;

extern u32 D_80057240;
extern unsigned char D_800C6BA0[];
extern unsigned char D_800C6BB0[];
extern void func_80047EB0(void *queue, void *buffer, int count);
extern int func_80048F00(void *queue, void *message, int flags);
extern int func_80048DD0(void *queue, void **message, int flags);

static __inline__ void create_access_queue(void) {
    D_80057240 = 1;
    func_80047EB0(D_800C6BB0, D_800C6BA0, 1);
    func_80048F00(D_800C6BB0, 0, 0);
}

void func_80050F04(void) {
    void *message;

    if (D_80057240 == 0) {
        create_access_queue();
    }
    func_80048DD0(D_800C6BB0, &message, 1);
}
