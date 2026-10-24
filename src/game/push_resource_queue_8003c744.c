extern int D_800C4018;
extern int D_800C401C;
extern int D_800C4014;
extern unsigned char *D_800C4020;
extern void func_8004312C(void *destination, const void *source, int size);

int func_8003C744(const void *entry)
{
    int index = D_800C4018;
    int next = (index + 1) % D_800C401C;

    if (next == D_800C4014) {
        return 0;
    }
    func_8004312C(D_800C4020 + index * 8, entry, 8);
    D_800C4018 = next;
    return 1;
}
