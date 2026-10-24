typedef int s32;
typedef double f64;

extern void func_8003A880(f64 x, f64 *s, f64 *c);

f64 func_8003AC18(f64 x) {
    s32 q;
    f64 s, c;

    q = (s32)(x / 3.141592653589793 + ((x >= 0.0) ? 0.5 : -0.5));
    x -= q * 3.141592653589793;
    if ((x < 1e-08) & (-1e-08 < x)) {
        return x;
    }
    func_8003A880(x, &s, &c);
    return s / c;
}
