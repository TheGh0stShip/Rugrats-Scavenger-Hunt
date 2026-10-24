typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern s32 D_80053410;
extern u32 D_80053424;
extern s32 D_80053440[16];
extern void *D_8005341C[2];
extern u32 func_80013544(s32);
extern void *func_8001A2B0(u32);

void func_80019C9C(void) {
    s32 i;
    u32 n;

    D_80053410 = 0;
    if (D_80053424 == 0) {
        for (i = 0; i < 16; i++) {
            n = func_80013544(D_80053440[i]);
            if (D_80053424 < n) {
                D_80053424 = n;
            }
        }
    }
    D_8005341C[0] = func_8001A2B0(D_80053424);
    D_8005341C[1] = func_8001A2B0(D_80053424);
}

