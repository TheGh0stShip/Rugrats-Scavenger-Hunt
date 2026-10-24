typedef unsigned short u16;
typedef int s32;

extern u16 D_80053978[4];
extern s32 func_80043554(u16);
extern void func_800434F8(u16);

void func_8001F30C(void) {
    s32 i;
    for (i = 0; i < 4; i++) {
        if (func_80043554(D_80053978[i])) {
            func_800434F8(D_80053978[i]);
        }
    }
}
