typedef signed int s32;
typedef float f32;

typedef struct ALDelay {
    char padding_00[0x10];
    f32 rate_increment;
    f32 rate_value;
    s32 rate_delta;
    f32 rate_gain;
} ALDelay;

f32 func_8004C808(ALDelay *delay, s32 count)
{
    f32 value;

    delay->rate_value += delay->rate_increment * count;
    delay->rate_value = delay->rate_value > 2.0
                            ? delay->rate_value - (2.0 * 2)
                            : delay->rate_value;
    value = delay->rate_value;
    value = value < 0 ? -value : value;
    value -= 2.0 / 2;
    return delay->rate_gain * value;
}
