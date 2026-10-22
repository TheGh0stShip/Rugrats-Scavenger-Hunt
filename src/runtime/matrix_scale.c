typedef struct Matrix Matrix;

struct Matrix {
    unsigned int words[16];
};

extern void func_80046A30(float matrix[4][4]);
extern void func_800468F0(float matrix[4][4], Matrix *result);

void func_80047510(float matrix[4][4], float x, float y, float z)
{
    func_80046A30(matrix);
    matrix[0][0] = x;
    matrix[1][1] = y;
    matrix[2][2] = z;
    matrix[3][3] = 1.0f;
}

void func_80047570(Matrix *result, float x, float y, float z)
{
    float matrix[4][4];

    func_80047510(matrix, x, y, z);
    func_800468F0(matrix, result);
}
