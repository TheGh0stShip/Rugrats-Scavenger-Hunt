typedef signed char s8;
typedef int s32;
typedef unsigned int u32;

extern s8 D_800C3E74;

extern char *func_8003931C(double value, s32 digit_count, s32 *decimal_point, s32 *sign);
extern void *func_80038D90(void *destination, const void *source, u32 length);
extern void func_800391BC(char *digits, s32 digit_count, s32 exponent, char *output);
extern char *func_8003ADD0(char *destination, const char *source);

char *func_800396E0(double value, s32 digit_count, char *buffer)
{
    s32 decimal_point;
    s32 sign;
    char *digits;
    char *output;

    digits = func_8003931C(value, digit_count, &decimal_point, &sign);
    output = buffer;
    if (sign) {
        *output++ = '-';
    }
    if (decimal_point == 0x7FFFFFFF) {
        func_8003ADD0(output, digits);
        return buffer;
    }
    if (decimal_point > 0) {
        goto positive;
    }
    if (decimal_point < -3) {
        goto exponential;
    }
    *output++ = '0';
    *output++ = '.';
    while (decimal_point != 0) {
        *output++ = '0';
        decimal_point++;
    }
    func_80038D90(output, digits, digit_count);
    output += digit_count;
trim:
    if (!D_800C3E74) {
        if (output[-1] == '0') {
            do {
                output--;
            } while (output[-1] == '0');
        }
        output -= (output[-1] == '.');
    }
    *output = 0;
    return buffer;
positive:
    if (decimal_point < digit_count) {
        while (--digit_count != -1) {
            *output++ = *digits++;
            if (--decimal_point == 0) {
                *output++ = '.';
            }
        }
        goto trim;
    }
    if (digit_count + 1 < decimal_point) {
        goto exponential;
    }
    func_80038D90(output, digits, decimal_point);
    output[digit_count] = '0';
    if (!D_800C3E74) {
        output[decimal_point] = 0;
    } else {
        output[decimal_point] = '.';
        (output + decimal_point)[1] = 0;
    }
    return buffer;
exponential:
    func_800391BC(digits, digit_count, decimal_point - 1, output);
    return buffer;
}
