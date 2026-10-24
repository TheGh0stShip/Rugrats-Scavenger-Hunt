typedef int s32;

extern s32 D_80052F18;
extern void func_8000771C(void *object);
extern void func_8000715C(void);

void func_8000711C(void *object) {
    if (D_80052F18 == 2) {
        func_8000771C(object);
    } else {
        func_8000715C();
    }
}
