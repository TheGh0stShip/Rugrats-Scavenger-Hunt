typedef float f32;

extern f32 sqrtf(f32 value);
extern void func_80046A30(f32 matrix[4][4]);

#ifndef GU_LOOK_AT_LINKAGE
#define GU_LOOK_AT_LINKAGE
#endif

GU_LOOK_AT_LINKAGE void func_800461E0(f32 matrix[4][4], f32 eye_x, f32 eye_y, f32 eye_z,
                   f32 at_x, f32 at_y, f32 at_z,
                   f32 up_x, f32 up_y, f32 up_z)
{
    f32 length, look_x, look_y, look_z, right_x, right_y, right_z;

    func_80046A30(matrix);
    look_x = at_x - eye_x;
    look_y = at_y - eye_y;
    look_z = at_z - eye_z;
    length = -1.0 / sqrtf(look_x * look_x + look_y * look_y + look_z * look_z);
    look_x *= length;
    look_y *= length;
    look_z *= length;
    right_x = up_y * look_z - up_z * look_y;
    right_y = up_z * look_x - up_x * look_z;
    right_z = up_x * look_y - up_y * look_x;
    length = 1.0 / sqrtf(right_x * right_x + right_y * right_y + right_z * right_z);
    right_x *= length;
    right_y *= length;
    right_z *= length;
    up_x = look_y * right_z - look_z * right_y;
    up_y = look_z * right_x - look_x * right_z;
    up_z = look_x * right_y - look_y * right_x;
    length = 1.0 / sqrtf(up_x * up_x + up_y * up_y + up_z * up_z);
    up_x *= length;
    up_y *= length;
    up_z *= length;
    matrix[0][0] = right_x;
    matrix[1][0] = right_y;
    matrix[2][0] = right_z;
    matrix[3][0] = -(eye_x * right_x + eye_y * right_y + eye_z * right_z);
    matrix[0][1] = up_x;
    matrix[1][1] = up_y;
    matrix[2][1] = up_z;
    matrix[3][1] = -(eye_x * up_x + eye_y * up_y + eye_z * up_z);
    matrix[0][2] = look_x;
    matrix[1][2] = look_y;
    matrix[2][2] = look_z;
    matrix[3][2] = -(eye_x * look_x + eye_y * look_y + eye_z * look_z);
    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;
    matrix[3][3] = 1;
}
