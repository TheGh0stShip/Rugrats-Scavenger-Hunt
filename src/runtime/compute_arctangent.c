typedef int s32;
typedef long long s64;
typedef double f64;

extern void func_80038950(s64 x, s64 y, f64 *out);

f64 func_80038B08(f64 v) {
    s32 sign;
    s64 x;
    s64 y;
    f64 r;

    if (v < 0.0) {
        v = -v;
        sign = -1;
    } else {
        sign = 1;
    }
    if (1e-07 < v) {
        if (v >= 1.0) {
            y = 0x1000000000000000LL;
            x = (s64)(1.0 / v * 1152921504606846976.0);
        } else {
            x = 0x1000000000000000LL;
            y = (s64)(v * 1152921504606846976.0);
        }
        func_80038950(x, y, &r);
    } else {
        r = v;
    }
    return (sign < 0) ? -r : r;
}
