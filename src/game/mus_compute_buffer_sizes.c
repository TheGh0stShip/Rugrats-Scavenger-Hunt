extern unsigned int D_800C42E0;
extern unsigned int D_800C42E4;
extern unsigned int D_800C42E8;
extern unsigned int D_800C42EC;

unsigned int func_80042F30(int a, int b, unsigned int c, unsigned int d)
{
    unsigned int n;

    n = (unsigned int)(a * b + c - 1) / c;
    D_800C42E0 = (n / 184 + 1) * 184;
    D_800C42E4 = D_800C42E0 - 184;
    D_800C42E8 = D_800C42E0 + 184;
    D_800C42EC = (D_800C42E0 * d) / 100;
    return D_800C42E0 + 184 + D_800C42EC;
}
