typedef struct Matrix Matrix;

struct Matrix {
    unsigned int words[16];
};

extern void func_80046A30(float matrix[4][4]);
extern void func_800468F0(float matrix[4][4], Matrix *result);
extern float func_80046090(float angle);
extern float func_800475F0(float angle);

void func_80046EB0(float matrix[4][4], unsigned short *normalization,
                   float field_of_view, float aspect, float near, float far,
                   float scale)
{
    float cotangent;
    int row;
    int column;

    func_80046A30(matrix);

    field_of_view *= 3.1415926 / 180.0;
    cotangent = func_80046090(field_of_view / 2.0f) /
                func_800475F0(field_of_view / 2.0f);

    matrix[0][0] = cotangent / aspect;
    matrix[1][1] = cotangent;
    matrix[2][2] = (near + far) / (near - far);
    matrix[2][3] = -1.0f;
    matrix[3][2] = (2.0f * near * far) / (near - far);
    matrix[3][3] = 0.0f;

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            matrix[row][column] *= scale;
        }
    }

    if (normalization != 0) {
        if (near + far <= 2.0) {
            *normalization = 0xFFFF;
        } else {
            *normalization = (unsigned short)((2.0 * 65536.0) / (near + far));
            if (*normalization <= 0) {
                *normalization = 1;
            }
        }
    }
}

void func_80047060(Matrix *result, unsigned short *normalization,
                   float field_of_view, float aspect, float near, float far,
                   float scale)
{
    float matrix[4][4];

    func_80046EB0(matrix, normalization, field_of_view, aspect, near, far,
                  scale);
    func_800468F0(matrix, result);
}
