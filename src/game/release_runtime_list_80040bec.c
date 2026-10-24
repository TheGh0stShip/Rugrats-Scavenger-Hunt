extern unsigned char *D_80055AF4;
extern void func_8004CA4C(void *entry);
extern void func_8004CA2C(void *entry, void *list);

void func_80040BEC(void)
{
    void *entry = *(void **)(D_80055AF4 + 0x14);

    while (entry != 0) {
        func_8004CA4C(entry);
        func_8004CA2C(entry, D_80055AF4 + 4);
        entry = *(void **)(D_80055AF4 + 0x14);
    }
}
