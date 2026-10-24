typedef signed char s8;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

extern s8 D_800C3E74;
extern u8 D_80055896;

extern void *func_80038D90(void *destination, const void *source, u32 length);

void func_800391BC(char *digits, s32 digit_count, s32 exponent, char *output)
{
    s32 count;

    *output++ = digits[0];
    *output++ = '.';
    count = digit_count - 1;
    func_80038D90(output, digits + 1, count);
    output += count;
    if (!D_800C3E74) {
        while (output[-1] == '0') {
            output--;
        }
        output -= (output[-1] == '.');
    }
    *output++ = D_80055896;
    if (exponent < 0) {
        exponent = -exponent;
        *output++ = '-';
    } else {
        *output++ = '+';
    }
    if (exponent >= 100) {
        *output++ = exponent / 100 + '0';
        exponent %= 100;
    }
    *output++ = exponent / 10 + '0';
    exponent %= 10;
    output[0] = exponent + '0';
    output[1] = 0;
}
