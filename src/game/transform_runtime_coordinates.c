extern void func_80006C18(int, int, float *, float *);
extern void func_80006C64(void *, float, float, float);

void func_80006AE0(void *object, int x, int y, float scale) {
    float transformed_x;
    float transformed_y;

    func_80006C18(x, y, &transformed_x, &transformed_y);
    func_80006C64(object, scale, transformed_x, transformed_y);
}
