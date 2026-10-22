extern float sqrtf(float value);

void func_80046E00(float *x, float *y, float *z)
{
    float inverse_magnitude;

    inverse_magnitude = 1.0f /
        sqrtf((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
    *x *= inverse_magnitude;
    *y *= inverse_magnitude;
    *z *= inverse_magnitude;
}
