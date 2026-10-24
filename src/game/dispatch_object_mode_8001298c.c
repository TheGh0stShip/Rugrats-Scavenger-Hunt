typedef int s32;

typedef struct ModeObject {
    unsigned char pad_00[0xFC];
    s32 mode;
} ModeObject;

extern s32 D_800529F8;

extern void func_80012A14(ModeObject *obj);
extern void func_80012B9C(ModeObject *obj);
extern void func_80006E88(ModeObject *obj);
extern void func_8000711C(ModeObject *obj);

void func_8001298C(ModeObject *obj) {
    s32 mode = obj->mode;

    D_800529F8 = 2;
    do {
        switch (mode) {
        case 5:
            func_80012A14(obj);
            break;
        case 6:
            func_80012B9C(obj);
            break;
        case 2:
        case 3:
            func_80006E88(obj);
            break;
        case 0:
        case 12:
            break;
        }
    } while (0);
    func_8000711C(obj);
}
