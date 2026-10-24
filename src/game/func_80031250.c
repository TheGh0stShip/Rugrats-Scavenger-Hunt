typedef unsigned char u8;
typedef int s32;

typedef struct SceneObject {
    u8 pad_00[0x30];
    s32 value_30;
} SceneObject;

extern s32 D_80059DA8;
extern SceneObject *D_800B933C;
extern void func_80008240(s32 value);
extern void func_8000D768(s32 value);
extern void func_80019D44(void);
extern void func_80019BD4(s32 index);
extern void func_8000E17C(void);
extern void func_8001F064(void);
extern int func_8001EFEC(s32 first, s32 second);
extern void func_8001F30C(void);
extern void func_80027FD4(void);
extern void func_80028E9C(void);

void func_80031250(void) {
    s32 i;

    func_80008240(D_80059DA8);
    if (D_800B933C != 0) {
        func_8000D768(D_800B933C->value_30);
    }
    func_80019D44();
    for (i = 0; i < 0x211; i++) {
        func_80019BD4(i);
    }
    func_8000E17C();
    func_8001F064();
    func_8001EFEC(0, 0);
    func_8001F30C();
    func_80027FD4();
    func_80028E9C();
}
