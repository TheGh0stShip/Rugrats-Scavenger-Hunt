typedef int s32;
typedef double f64;

extern f64 func_8003AFB0(s32 type, char *name, f64 a, f64 b, f64 ret);
extern f64 func_80038B08(f64 x);

f64 func_80038C20(f64 y, f64 x) {
    f64 r;

    if ((x == 0.0) & (y == 0.0)) {
        return func_8003AFB0(1, "atan2", x, y, 0.0);
    }
    if (x == 0.0) {
        return (y > 0.0) ? 1.5707963267948966 : -1.5707963267948966;
    }
    r = func_80038B08(y / x);
    if (x > 0.0) {
        return r;
    }
    if (y == 0.0) {
        return 3.141592653589793;
    }
    if (y > 0.0) {
        return r + 3.141592653589793;
    }
    return r - 3.141592653589793;
}
