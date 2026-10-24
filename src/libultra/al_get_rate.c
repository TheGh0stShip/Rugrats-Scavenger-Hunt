typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef double f64;
typedef float f32;

static f64 scale_power_two(f64 value, s32 exponent)
{
    s32 scale;
    if (exponent) {
        scale = 1 << exponent;
        value *= (f64)scale;
    }
    return value;
}

static f64 split_mantissa(f64 value, s32 *exponent)
{
    f64 absolute;
    *exponent = 0;
    if (value == 0.0) return value;
    absolute = value > 0.0 ? value : -value;
    for (; absolute >= 1.0; absolute *= 0.5) ++*exponent;
    for (; absolute < 0.5; absolute += absolute) --*exponent;
    return value > 0.0 ? absolute : -absolute;
}

s16 func_8004B564(f64 volume, f64 target, s32 count, u16 *rate_fraction)
{
    s16 integer;
    f64 inverse = 1.0 / count;
    f64 epsilon;
    f64 rate;
    f64 factor;
    f64 mantissa;
    s32 inverse_integer;
    s32 exponent;
    s32 index;

    if (count == 0) {
        if (target >= volume) {
            *rate_fraction = 0xFFFF;
            return 0x7FFF;
        } else {
            *rate_fraction = 0;
            return 0;
        }
    }
    if (target < 1.0) {
        target = 1.0;
    }
    if (volume <= 0) {
        volume = 1;
    }
    {
        f64 logarithms[] = {
            -0.912537, -0.752072, -0.607683, -0.476438,
            -0.356144, -0.245112, -0.142019, -0.045804
        };
        inverse_integer = (s32)scale_power_two(inverse, 30);
        mantissa = split_mantissa(target / volume, &exponent);
        index = (s32)scale_power_two(mantissa, 4);
        epsilon = (logarithms[index - 8] + exponent) * 0.69314718055994530942;
        epsilon /= scale_power_two(1, 30);
        factor = 1.0 + epsilon;
        rate = 1.0;
        while (inverse_integer) {
            if (inverse_integer & 1) {
                rate = rate * factor;
            }
            factor *= factor;
            inverse_integer >>= 1;
        }
    }
    rate *= (rate *= (rate *= rate));
    integer = (s16)rate;
    *rate_fraction = (s16)(0xFFFF * (rate - (f32)integer));
    return (s16)rate;
}
