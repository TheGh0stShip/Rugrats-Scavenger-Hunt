typedef unsigned char u8;
extern void *D_800C3FF8;
void *func_8003C320(void *object) {
    if (object != 0 && *(int *)((u8 *)object + 0x10) < 0) D_800C3FF8 = object;
    return D_800C3FF8;
}
