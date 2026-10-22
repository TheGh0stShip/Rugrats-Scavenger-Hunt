typedef unsigned int u32;

typedef struct Matrix Matrix;

struct Matrix {
    u32 words[16];
};

void func_800468F0(float source[4][4], Matrix *result)
{
    int row;
    int pair;
    int first;
    int second;
    int *integer_part;
    int *fraction_part;

    integer_part = (int *)&result->words[0];
    fraction_part = (int *)&result->words[8];

    for (row = 0; row < 4; row++) {
        for (pair = 0; pair < 2; pair++) {
            first = (int)(source[row][pair * 2] * 65536.0f);
            second = (int)(source[row][pair * 2 + 1] * 65536.0f);
            *integer_part++ = (first & 0xFFFF0000) |
                              ((second >> 16) & 0xFFFF);
            *fraction_part++ = ((first << 16) & 0xFFFF0000) |
                               (second & 0xFFFF);
        }
    }
}

void func_80046988(float result[4][4], Matrix *source)
{
    int row;
    int pair;
    u32 first;
    u32 second;
    u32 *integer_part;
    u32 *fraction_part;
    int first_signed;
    int second_signed;

    integer_part = &source->words[0];
    fraction_part = &source->words[8];

    for (row = 0; row < 4; row++) {
        for (pair = 0; pair < 2; pair++) {
            first = (*integer_part & 0xFFFF0000) |
                    ((*fraction_part >> 16) & 0xFFFF);
            second = ((*integer_part++ << 16) & 0xFFFF0000) |
                     (*fraction_part++ & 0xFFFF);
            first_signed = *(int *)&first;
            second_signed = *(int *)&second;
            result[row][pair * 2] = first_signed * (1.0f / 65536.0f);
            result[row][pair * 2 + 1] = second_signed * (1.0f / 65536.0f);
        }
    }
}

void func_80046A30(float matrix[4][4])
{
    int row;
    int column;

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            if (row == column) {
                matrix[row][column] = 1.0f;
            } else {
                matrix[row][column] = 0.0f;
            }
        }
    }
}

void func_80046A80(Matrix *result)
{
    float matrix[4][4];

    func_80046A30(matrix);
    func_800468F0(matrix, result);
}
