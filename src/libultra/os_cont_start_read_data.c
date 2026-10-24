typedef int s32;
typedef unsigned char u8;

extern u8 D_800C4640;
extern unsigned int D_800C4600[];

extern void func_8004DE24(void);
extern void func_8004DC54(void);
extern s32 func_8004DD10(s32 direction, void *buffer);
extern s32 func_80048DD0(void *queue, void *message, s32 flags);
extern void func_8004DE90(void);

s32 func_8004DB30(void *queue) {
    s32 result = 0;

    func_8004DE24();
    if (D_800C4640 != 1) {
        func_8004DC54();
        result = func_8004DD10(1, D_800C4600);
        func_80048DD0(queue, 0, 1);
    }
    result = func_8004DD10(0, D_800C4600);
    D_800C4640 = 1;
    func_8004DE90();
    return result;
}
