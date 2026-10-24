/* Target: func_8003A880  VRAM 0x8003A880  ROM 0x3B480
 * Size: 0x220  Pipeline: KMC GCC 2.7.2 -O1 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */

typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef double f64;

extern const f64 D_80003250;
extern const f64 D_80003258;
extern s64 D_80055670[];

void func_8003A880(f64 a, f64 *s, f64 *c) {
    s64 x = 0x09B74EDA8435E580LL;
    s64 y = 0;
    s64 z;
    s64 nx;
    s32 i;

    z = (s64)(a * D_80003250);
    for (i = 0; i < 52; i++) {
        if (z >= 0) {
            nx = x - (y >> i);
            y += x >> i;
            z -= D_80055670[i];
        } else {
            nx = x + (y >> i);
            y -= x >> i;
            z += D_80055670[i];
        }
        x = nx;
    }
    *s = (f64)y / D_80003258;
    *c = (f64)x / D_80003258;
}

