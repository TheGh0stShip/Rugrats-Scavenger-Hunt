typedef unsigned char u8;
extern void *D_800C4004;
extern void func_8003C744(u8 *mode);
void func_8003C420(void *object) {
    struct {
        u8 mode;
        u8 padding[3];
        void *volatile argument;
    } locals;

    locals.mode = 2;
    locals.argument = object;
    D_800C4004 = object;
    func_8003C744(&locals.mode);
}
