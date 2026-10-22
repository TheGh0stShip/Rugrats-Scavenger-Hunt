void func_80046B70(float left[4][4], float right[4][4], float result[4][4])
{
    int row;
    int column;
    int element;
    float temporary[4][4];

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            temporary[row][column] = 0.0f;
            for (element = 0; element < 4; element++) {
                temporary[row][column] +=
                    left[row][element] * right[element][column];
            }
        }
    }

    for (row = 0; row < 4; row++) {
        for (column = 0; column < 4; column++) {
            result[row][column] = temporary[row][column];
        }
    }
}

void func_80046C4C(float matrix[4][4], float x, float y, float z,
                   float *result_x, float *result_y, float *result_z)
{
    *result_x = matrix[0][0] * x + matrix[1][0] * y +
                matrix[2][0] * z + matrix[3][0];
    *result_y = matrix[0][1] * x + matrix[1][1] * y +
                matrix[2][1] * z + matrix[3][1];
    *result_z = matrix[0][2] * x + matrix[1][2] * y +
                matrix[2][2] * z + matrix[3][2];
}
