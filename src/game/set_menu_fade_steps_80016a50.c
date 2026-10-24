/* func_80016A50: store a target level, a step count of 5, and the
 * rounded-up per-step delta from the current level (byte 0). */

typedef unsigned char u8;
typedef signed short s16;

void func_80016A50(u8 *p, s16 v) {
    p[1] = v;
    p[2] = 5;
    p[3] = (v - p[0] + p[2] - 1) / p[2];
}
