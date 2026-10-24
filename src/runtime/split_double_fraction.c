typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef double f64;

typedef union {
    f64 value;
    u32 words[2];
    u8 bytes[8];
} DoubleBits;

f64 func_8003A720(f64 value, f64 *integer_part)
{
    DoubleBits *bits = (DoubleBits *)integer_part;
    s32 exponent;
    s32 byte_index;

    bits->value = value;
    exponent = ((bits->words[0] >> 20) & 0x7FF) - 0x3FF;
    if (exponent < 0) {
        bits->words[0] = 0;
        bits->words[1] = 0;
        return value;
    }
    if (exponent >= 52) {
        bits->value = value;
        return 0.0;
    }
    exponent += 12;
    byte_index = exponent >> 3;
    bits->bytes[byte_index] &= 0xFF << (8 - (exponent & 7));
    while (byte_index < 7) {
        byte_index++;
        bits->bytes[byte_index] = 0;
    }
    return value - bits->value;
}
