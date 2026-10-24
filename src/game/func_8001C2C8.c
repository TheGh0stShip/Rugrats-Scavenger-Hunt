typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

extern void *D_8005B214;
extern s32 D_8005385C;
extern u32 func_800166B0(void *);

s32 func_8001C2C8(void) {
    s32 ret = 0;
    u32 buttons = func_800166B0(D_8005B214);

    if (D_8005385C == 0) {
        D_8005385C = 45;
    } else {
        D_8005385C--;
        if (D_8005385C == 0) {
            ret = 1;
            D_8005385C = 0;
        }
    }
    if (buttons & 0x10) {
        ret = 1;
    }
    return ret;
}

