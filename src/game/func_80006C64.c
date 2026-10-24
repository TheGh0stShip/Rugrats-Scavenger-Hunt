typedef float f32;
typedef double f64;

extern f64 func_8003AAA0(f64 x);
extern f64 func_8003AB80(f64 x);

void func_80006C64(f32 *out, f32 angle, f32 radius, f32 y) {
    f32 c;
    f64 rad = angle * 6.283185307 / 360.0;

    out[0] = func_8003AAA0(rad);
    out[2] = c = func_8003AB80(rad);
    out[1] = y;
    c *= radius;
    out[2] = c;
    out[0] *= radius;
}
