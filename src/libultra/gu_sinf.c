typedef signed int s32;
typedef unsigned int u32;
typedef float f32;

typedef union DoubleBits {
    struct {
        u32 hi;
        u32 lo;
    } word;
    double value;
} DoubleBits;

#define ROUND(value) ((s32)(((value) >= 0.0) ? ((value) + 0.5) : ((value) - 0.5)))

static const DoubleBits coefficients[] = {
    {{0xBFC55554, 0xBC83656D}},
    {{0x3F8110ED, 0x3804C2A0}},
    {{0xBF29F6FF, 0xEEA56814}},
    {{0x3EC5DBDF, 0x0E314BFE}},
};
static const DoubleBits reciprocalPi = {{0x3FD45F30, 0x6DC9C883}};
static const DoubleBits piHigh = {{0x400921FB, 0x50000000}};
static const DoubleBits piLow = {{0x3E6110B4, 0x611A6263}};
static const DoubleBits padding = {{0, 0}};
extern f32 sine_nan;
extern f32 floating_zero;

f32 func_800475F0(f32 x)
{
    double dx;
    double square;
    double polynomial;
    double rounded;
    double result;
    s32 quadrant;
    s32 bits;
    s32 exponent;

    bits = *(s32 *)&x;
    exponent = bits >> 22;
    exponent &= 0x1FF;
    if (exponent < 0xFF) {
        dx = x;
        if (exponent >= 0xE6) {
            square = dx * dx;
            polynomial = ((coefficients[3].value * square + coefficients[2].value) * square
                          + coefficients[1].value) * square + coefficients[0].value;
            result = dx + (dx * square) * polynomial;
            return (f32)result;
        }
        return x;
    }
    if (exponent < 0x136) {
        dx = x;
        rounded = dx * reciprocalPi.value;
        quadrant = ROUND(rounded);
        rounded = quadrant;
        dx = dx - rounded * piHigh.value;
        dx = dx - rounded * piLow.value;
        square = dx * dx;
        polynomial = ((coefficients[3].value * square + coefficients[2].value) * square
                      + coefficients[1].value) * square + coefficients[0].value;
        result = dx + (dx * square) * polynomial;
        if ((quadrant & 1) == 0) {
            return (f32)result;
        }
        return -(f32)result;
    }
    if (x != x) {
        return sine_nan;
    }
    return floating_zero;
}
