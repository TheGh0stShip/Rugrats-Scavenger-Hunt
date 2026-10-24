/* func_80006C18: choose spawn offsets from two flags. */
typedef int s32;
typedef float f32;

extern f32 D_80052A10;
extern f32 D_80052A14;
extern f32 D_80052A18;
extern f32 D_80052A1C;

void func_80006C18(s32 a, s32 b, f32 *x, f32 *y) {
    *y = 0.0f;
    if (a == 0) {
        if (b != 0) {
            *x = D_80052A14;
            *y = D_80052A10;
        } else {
            *x = D_80052A18;
        }
    } else {
        *x = D_80052A1C;
    }
}
