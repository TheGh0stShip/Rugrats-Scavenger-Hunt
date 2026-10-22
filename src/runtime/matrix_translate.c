typedef struct Matrix Matrix;

struct Matrix {
    unsigned int words[16];
};

extern void func_80046A30(float matrix[4][4]);
extern void func_800468F0(float matrix[4][4], Matrix *result);

void func_80047790(float matrix[4][4], float x, float y, float z)
{
    func_80046A30(matrix);
    matrix[3][0] = x;
    matrix[3][1] = y;
    matrix[3][2] = z;
}

void func_800477E4(Matrix *result, float x, float y, float z)
{
    float matrix[4][4];

    func_80047790(matrix, x, y, z);
    func_800468F0(matrix, result);
}
