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
