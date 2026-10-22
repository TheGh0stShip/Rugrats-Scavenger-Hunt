extern void func_800082B8(float *matrix);
extern void func_800468F0(float *source, void *destination);
extern void func_80047E00(void *address, int size);

void func_80008DD0(void *object) {
    float matrix[16];

    func_800082B8(matrix);
    func_800468F0(matrix, object);
    func_80047E00(object, 0x40);
}
