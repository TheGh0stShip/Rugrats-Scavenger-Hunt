typedef float f32;

static f32 degrees_to_radians = 3.1415926 / 180.0;

extern f32 sqrtf(f32 value);
extern f32 func_80046090(f32 angle);
extern f32 func_800475F0(f32 angle);
extern void func_80046A30(f32 matrix[4][4]);
extern void func_80046E00(f32 *x, f32 *y, f32 *z);

#ifndef GU_ALIGN_LINKAGE
#define GU_ALIGN_LINKAGE
#endif

GU_ALIGN_LINKAGE void func_80045CF0(f32 matrix[4][4], f32 angle,
                                    f32 x, f32 y, f32 z)
{
    f32 sine;
    f32 cosine;
    f32 horizontal;
    f32 inverse_horizontal;

    func_80046E00(&x, &y, &z);
    angle *= degrees_to_radians;
    sine = func_800475F0(angle);
    cosine = func_80046090(angle);
    horizontal = sqrtf(x * x + z * z);
    func_80046A30(matrix);
    if (horizontal != 0.0f) {
        inverse_horizontal = 1.0f / horizontal;
        matrix[0][0] = (-z * cosine - sine * y * x) * inverse_horizontal;
        matrix[1][0] = (z * sine - cosine * y * x) * inverse_horizontal;
        matrix[2][0] = -x;
        matrix[3][0] = 0.0f;
        matrix[0][1] = sine * horizontal;
        matrix[1][1] = cosine * horizontal;
        matrix[2][1] = -y;
        matrix[3][1] = 0.0f;
        matrix[0][2] = (cosine * x - sine * y * z) * inverse_horizontal;
        matrix[1][2] = (-sine * x - cosine * y * z) * inverse_horizontal;
        matrix[2][2] = -z;
        matrix[3][2] = 0.0f;
        matrix[0][3] = 0.0f;
        matrix[1][3] = 0.0f;
        matrix[2][3] = 0.0f;
        matrix[3][3] = 1.0f;
    }
}
