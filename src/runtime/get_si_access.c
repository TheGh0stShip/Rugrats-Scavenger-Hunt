typedef unsigned int u32;

extern u32 D_800570F0;
extern unsigned char D_800C4590[];
extern unsigned char D_800C45A0[];
extern void func_80047EB0(void *queue, void *buffer, int count);
extern int func_80048F00(void *queue, void *message, int flags);
extern int func_80048DD0(void *queue, void **message, int flags);

static __inline__ void create_access_queue(void) {
    D_800570F0 = 1;
    func_80047EB0(D_800C45A0, D_800C4590, 1);
    func_80048F00(D_800C45A0, 0, 0);
}

void func_8004DE24(void) {
    void *message;

    if (D_800570F0 == 0) {
        create_access_queue();
    }
    func_80048DD0(D_800C45A0, &message, 1);
}
