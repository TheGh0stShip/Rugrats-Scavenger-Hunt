typedef int s32;
typedef long long s64;
typedef double f64;

extern s64 D_80055670[];

void func_80038950(s64 a, s64 y, f64 *out) {
    s64 x = a;
    s64 z = 0;
    s64 nx;
    s32 i;

    for (i = 0; i < 52; i++) {
        if (y >= 0) {
            nx = x + (y >> i);
            y -= x >> i;
            z += D_80055670[i];
        } else {
            nx = x - (y >> i);
            y += x >> i;
            z -= D_80055670[i];
        }
        x = nx;
    }
    *out = (f64)z / 1152921504606846976.0;
}
