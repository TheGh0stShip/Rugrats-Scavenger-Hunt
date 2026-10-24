typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef float f32;

f32 func_8004B7E4(f32 initial_volume, s32 samples, s16 rate_mantissa,
                  u16 rate_fraction)
{
    f32 rate;
    f32 multiplier;
    s32 i;

    samples >>= 3;
    if (samples == 0) {
        return initial_volume;
    }
    rate = ((f32)(rate_mantissa << 16) + (f32)rate_fraction) / 65536;
    multiplier = 1.0;
    for (i = 0; i < 32; i++) {
        if (samples & 1) {
            multiplier *= rate;
        }
        samples >>= 1;
        if (samples == 0) {
            break;
        }
        rate *= rate;
    }
    initial_volume *= multiplier;
    return initial_volume;
}
