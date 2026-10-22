typedef unsigned char u8;
extern void *D_800C3FFC;
void func_8003C348(void *object) {
    if (object != 0 && *(int *)((u8 *)object + 0x10) < 0) D_800C3FFC = object;
}
