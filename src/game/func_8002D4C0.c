typedef unsigned short u16;
typedef int s32;

struct Object_8002D4C0 {
    char pad_00[0xF4];
    s32 index;
};

extern s32 D_80052F10;
extern s32 D_80057D5C;
extern struct Object_8002D4C0 *D_8005B214;
extern u16 D_800544D4[2];
extern void func_8002C714(s32, struct Object_8002D4C0 *);

void func_8002D4C0(void) {
    struct Object_8002D4C0 *object;

    if (D_80052F10 != 0) {
        object = D_8005B214;
        if (object->index < D_80057D5C) {
            func_8002C714((D_800544D4[0] << 16) | D_800544D4[1], object);
        }
    }
}
