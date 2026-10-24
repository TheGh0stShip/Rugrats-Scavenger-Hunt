typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16; typedef int s32; typedef unsigned int u32;

extern s32 D_80057D5C;
extern void *func_800055A4(s32);
extern s32 func_80022760(void *);
extern void func_80024730(void *);

s32 func_80024660(void *obj) {
    void *found = 0;
    s32 ret = 0;
    s32 i;

    if (obj != 0) {
        if (func_80022760(obj)) {
            found = obj;
        }
    } else {
        for (i = 0; i < D_80057D5C; i++) {
            if (func_80022760(func_800055A4(i))) {
                if (found != 0) {
                    break;
                }
                found = func_800055A4(i);
            }
        }
    }
    if (found != 0) {
        func_80024730(found);
        ret = 1;
    }
    return ret;
}

