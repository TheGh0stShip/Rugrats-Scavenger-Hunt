typedef signed short s16;
typedef signed int s32;
typedef unsigned short u16;

s16 func_8003FAEC(s32 base, s32 count, s32 step, s32 fraction)
{
    s32 result;

    count >>= 3;
    if (count != 0) {
        result = (s16)(base + ((((u16)fraction * count) >> 16) + ((s16)step * count)));
    } else {
        result = (s16)base;
    }
    return result;
}
