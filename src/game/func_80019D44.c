typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80053414[2];
extern void *D_8005341C[2];
extern void *D_8005A990[];
extern void func_8001A3EC(void *);

void func_80019D44(void) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (D_8005341C[i] != 0) {
            if (D_80053414[i] != 0) {
                D_8005A990[D_80053414[i]] = 0;
                D_80053414[i] = 0;
            }
            func_8001A3EC(D_8005341C[i]);
            D_8005341C[i] = 0;
        }
    }
}

