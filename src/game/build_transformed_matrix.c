typedef float Matrix[4][4];
typedef int FixedMatrix[4][4];

extern void func_80047790(Matrix matrix, float x, float y, float z);
extern void func_80047510(Matrix matrix, float x, float y, float z);
extern void func_80046B70(Matrix left, Matrix right, Matrix output);
extern void func_800468F0(Matrix source, FixedMatrix *destination);

void func_80015CE0(
    FixedMatrix *destination,
    float translate_x,
    float translate_y,
    float translate_z,
    float scale_x,
    float scale_y,
    float scale_z
) {
    Matrix translation;
    Matrix scale;

    func_80047790(translation, translate_x, translate_y, translate_z);
    func_80047510(scale, scale_x, scale_y, scale_z);
    func_80046B70(scale, translation, translation);
    func_800468F0(translation, destination);
}
