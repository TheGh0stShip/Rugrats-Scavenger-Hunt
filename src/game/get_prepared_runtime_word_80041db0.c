extern unsigned char *D_80055AF4;
extern void func_80040DD0(void *entry);

int func_80041DB0(int index)
{
    int offset = index << 16;
    unsigned char *base;

    offset >>= 14;
    base = *(unsigned char **)(D_80055AF4 + 0x34);
    func_80040DD0(base + (offset + 0x24));
    base = *(unsigned char **)(D_80055AF4 + 0x34);
    offset += (int)base;
    return *(int *)(offset + 0x24);
}
