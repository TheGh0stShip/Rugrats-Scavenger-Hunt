extern unsigned char *D_80055AF4;

int func_80040CD4(int value)
{
    float scaled = (float)value * (float)*(int *)(D_80055AF4 + 0x40);
    float rounded = (float)((double)scaled / 1000000.0 + 0.5);

    return ((int)rounded) & -16;
}
