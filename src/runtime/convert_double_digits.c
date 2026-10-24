typedef int s32;
typedef unsigned int u32;
typedef double f64;

typedef union F64Bits {
    f64 d;
    u32 w[2];
} F64Bits;

extern char D_800C3E78[];
extern f64 D_80055898[9];
extern f64 D_800558E0[9];
extern char *strcpy(char *dst, const char *src);
extern f64 func_8003A720(f64 x, f64 *ip);
extern s32 func_80039138(char *buf, s32 exp, s32 ndigits);

char *func_8003931C(f64 value, s32 ndigits, s32 *exp, s32 *neg) {
    F64Bits u;
    volatile f64 t;
    f64 ip;
    char *p;
    s32 e;
    s32 n;
    s32 i;
    s32 bit;

    e = 0;
    bit = 0x100;
    *neg = 0;
    n = ndigits;
    if (ndigits >= 20) {
        ndigits = 20;
        n = 19;
    }
    u.d = value;
    switch (u.w[0] & 0x7FF00000) {
    case 0:
        p = D_800C3E78;
        for (i = 0; i < ndigits; i++) {
            *p++ = '0';
        }
        *exp = 1;
        return D_800C3E78;
    case 0x7FF00000:
        *exp = 0x7FFFFFFF;
        if ((u.w[0] & 0xFFFFF) != 0 || u.w[1] != 0) {
            p = strcpy(D_800C3E78, "NAN");
            *neg = 0;
            return p;
        }
        return strcpy(D_800C3E78, "INF");
    }

    if (value < 0.0) {
        value = -value;
        *neg = 1;
    }
    if (value >= 1.0) {
        for (i = 8; i >= 0; i--, bit >>= 1) {
            if (value >= D_80055898[i]) {
                value *= D_800558E0[i];
                e += bit;
            }
        }
    } else {
        for (i = 8; i >= 0; i--, bit >>= 1) {
            if (value < D_800558E0[i]) {
                value *= D_80055898[i];
                e -= bit;
            }
        }
        value *= 10.0;
        e--;
    }
    t = value;
normalize:
    if (t >= 10.0) {
        t *= 0.1;
        e++;
        goto normalize;
    }
    if (t < 1.0) {
        t *= 10.0;
        e--;
        goto normalize;
    }

    p = D_800C3E78;
    do {
        t = func_8003A720(t, &ip) * 10.0;
        *p++ = (s32)ip + '0';
    } while (--n >= 0);

    if (ndigits >= 20) {
        *exp = e + 1;
    } else {
        *exp = func_80039138(D_800C3E78, e, ndigits) + 1;
    }
    return D_800C3E78;
}
