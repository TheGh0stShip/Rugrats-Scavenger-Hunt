typedef int s32;

s32 func_80039138(char *digits, s32 exponent, s32 count)
{
    char *current;
    char *end;
    char digit;

    current = digits + count;
    if (*current < '5') {
        *current = 0;
        return exponent;
    }
    end = current;
    *end = 0;
    while (--count >= 0) {
        digit = *--current;
        if (digit != '9') {
            *current = digit + 1;
            return exponent;
        }
        *current = '0';
    }
    *current = '1';
    end[0] = '0';
    end[1] = 0;
    return exponent + 1;
}
