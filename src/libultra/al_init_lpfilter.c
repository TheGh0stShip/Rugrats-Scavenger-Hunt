typedef signed short s16;
typedef signed int s32;
typedef double f64;
typedef struct LowPass {
    s16 fc;
    s16 fgain;
    s32 alignment;
    s16 coefficients[16];
    void *state;
    s32 first;
} LowPass;

void func_800496F0(LowPass *filter)
{
    s32 i;
    s32 temporary;
    s16 frequency;
    f64 base;
    f64 coefficient;
    temporary = filter->fc * 16384;
    frequency = temporary >> 15;
    filter->fgain = 16384 - frequency;
    filter->first = 1;
    for (i = 0; i < 8; i++) filter->coefficients[i] = 0;
    filter->coefficients[i++] = frequency;
    coefficient = base = (f64)frequency / 16384;
    for (; i < 16; i++) {
        coefficient *= base;
        filter->coefficients[i] = (s16)(coefficient * 16384);
    }
}
