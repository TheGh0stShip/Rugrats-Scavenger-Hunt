typedef unsigned int u32;

extern u32 D_800C42F0;
extern void func_80043100(void *, int, int);
extern void func_8004CA80(void *, void *, int);

void func_80043060(void *buffer, int size) {
    func_80043100(buffer, 0, size);
    func_8004CA80(&D_800C42F0, buffer, size);
}
