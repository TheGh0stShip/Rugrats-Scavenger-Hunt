typedef struct {
    int offset;
    int size;
} bank_entry_t;

typedef struct {
    int count;
    int pad04;
    int pad08;
    int flags;
    int f10;
    int f14;
    bank_entry_t entries[1];
} bank_t;

extern bank_t *D_800C4008;

void func_8003C4AC(void *addr)
{
    int i;
    unsigned int base;
    bank_t *bank;

    bank = addr;
    base = (unsigned int)addr;
    if (bank->flags & 1)
        return;
    if (D_800C4008 == 0)
        D_800C4008 = bank;
    bank->flags = 1;
    bank->f10 = 0;
    bank->f14 = (int)bank + bank->f14;
    for (i = 0; i < bank->count; i++)
        bank->entries[i].offset = base + bank->entries[i].offset;
}
