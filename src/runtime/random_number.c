static unsigned int seed = 174823885;

int func_80047C20(void)
{
    unsigned int value;

    value = (seed << 2) + 2;
    value *= value + 1;
    value >>= 2;
    seed = value;
    return value;
}
