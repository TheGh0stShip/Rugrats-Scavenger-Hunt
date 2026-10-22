typedef struct Matrix Matrix;

struct Matrix {
    unsigned int words[16];
};

static float degrees_to_radians = 3.1415926f / 180.0f;

extern void func_80046A30(float matrix[4][4]);
extern void func_80046E00(float *x, float *y, float *z);
extern void func_800468F0(float matrix[4][4], Matrix *result);
extern float func_800475F0(float angle);
extern float func_80046090(float angle);

void func_80047230(float matrix[4][4], float angle, float x, float y, float z)
{
    float sine;
    float cosine;
    float xy;
    float yz;
    float zx;
    float value;

    func_80046E00(&x, &y, &z);
    angle *= degrees_to_radians;
    sine = func_800475F0(angle);
    cosine = func_80046090(angle);
    value = 1.0f - cosine;
    xy = x * y * value;
    yz = y * z * value;
    zx = z * x * value;

    func_80046A30(matrix);

    value = x * x;
    matrix[0][0] = value + cosine * (1.0f - value);
    matrix[2][1] = yz - x * sine;
    matrix[1][2] = yz + x * sine;

    value = y * y;
    matrix[1][1] = value + cosine * (1.0f - value);
    matrix[2][0] = zx + y * sine;
    matrix[0][2] = zx - y * sine;

    value = z * z;
    matrix[2][2] = value + cosine * (1.0f - value);
    matrix[1][0] = xy - z * sine;
    matrix[0][1] = xy + z * sine;
}

void func_8004738C(Matrix *result, float angle, float x, float y, float z)
{
    float matrix[4][4];

    func_80047230(matrix, angle, x, y, z);
    func_800468F0(matrix, result);
}
