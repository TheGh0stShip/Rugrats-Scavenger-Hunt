typedef unsigned char u8;
extern void func_8003C744(u8 *mode);
void func_8003C3F8(void *unused) {
    struct {
        u8 mode;
        u8 padding[3];
        void *volatile argument;
    } locals;

    locals.mode = 1;
    locals.argument = unused;
    func_8003C744(&locals.mode);
}
