typedef int s32;
typedef double f64;

extern char D_800C3E78[];
extern char *func_8003931C(f64 x, s32 ndigits, s32 *exp, s32 *neg);
extern s32 func_80039138(char *s, int, int);

char *func_80039634(f64 x, s32 prec, s32 *exp, s32 *neg) {
    char *p;
    s32 n;

    p = func_8003931C(x, 20, exp, neg);
    if (*exp != 0x7FFFFFFF) {
        n = *exp;
        n += prec;
        if (n >= 0) {
            if (n < 20) {
                *exp = func_80039138(D_800C3E78, *exp, n);
            } else {
                if (n > 40) {
                    n = 40;
                }
                n -= 20;
                p += 20;
                while (n > 0) {
                    *p = '0';
                    n--;
                    p++;
                }
            }
        }
    }
    return D_800C3E78;
}
