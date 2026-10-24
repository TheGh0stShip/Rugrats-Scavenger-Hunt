typedef signed int s32;
typedef double f64;

f64 func_8004B460(f64 value, s32 *exponent)
{
    f64 absolute;

    *exponent = 0;
    if (value == 0.0) {
        return value;
    }
    absolute = (value > 0.0) ? value : -value;
    for (; absolute >= 1.0; absolute *= 0.5) {
        ++*exponent;
    }
    for (; absolute < 0.5; absolute += absolute) {
        --*exponent;
    }
    return value > 0.0 ? absolute : -absolute;
}
