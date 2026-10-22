typedef struct Matrix Matrix;

struct Matrix {
    unsigned int words[16];
};

extern void func_800468F0(float matrix[4][4], Matrix *result);
extern void func_80046988(float matrix[4][4], Matrix *source);
extern void func_80046B70(float left[4][4], float right[4][4],
                          float result[4][4]);
extern void func_80046C4C(float matrix[4][4], float x, float y, float z,
                          float *result_x, float *result_y, float *result_z);

void func_80046D00(Matrix *left, Matrix *right, Matrix *result)
{
    float left_f[4][4];
    float right_f[4][4];
    float result_f[4][4];

    func_80046988(left_f, left);
    func_80046988(right_f, right);
    func_80046B70(left_f, right_f, result_f);
    func_800468F0(result_f, result);
}

void func_80046D74(Matrix *matrix, float x, float y, float z,
                   float *result_x, float *result_y, float *result_z)
{
    float matrix_f[4][4];

    func_80046988(matrix_f, matrix);
    func_80046C4C(matrix_f, x, y, z, result_x, result_y, result_z);
}
