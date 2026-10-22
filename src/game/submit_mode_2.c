typedef unsigned char u8;
extern void *D_800C4004;
extern void func_8003C744(u8 *mode);
void func_8003C420(void *object) {
    u8 mode = 2;
    D_800C4004 = object;
    func_8003C744(&mode);
}
