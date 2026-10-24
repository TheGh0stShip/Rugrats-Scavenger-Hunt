typedef int s32;
typedef double f64;

extern void func_8003A880(f64 x, f64 *s, f64 *c);

f64 func_8003AB80(f64 x) {
    s32 q;
    f64 s, c;
    f64 r;

    q = (s32)(x / 3.141592653589793 + ((x >= 0.0) ? 0.5 : -0.5));
    func_8003A880(x - q * 3.141592653589793, &s, &c);
    r = c;
    if (q & 1) {
        r = -r;
    }
    return r;
}
