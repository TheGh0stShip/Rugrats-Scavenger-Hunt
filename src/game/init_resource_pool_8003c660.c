extern void *D_800C4020;
extern int D_800C401C;
extern int D_800C4018;
extern int D_800C4014;
extern void *func_800430AC(int size);

void func_8003C660(int count)
{
    if (count < 0x40) {
        count = 0x40;
    } else if (count >= 0x401) {
        count = 0x400;
    }
    D_800C4020 = func_800430AC(count << 3);
    D_800C401C = count;
    D_800C4018 = 0;
    D_800C4014 = 0;
}
