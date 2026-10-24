typedef unsigned char u8;
typedef int s32;

extern void func_8001F044(s32);
extern s32 func_8001F138(s32);

s32 func_8001F0E8(s32 arg0, s32 arg1) {
    if (arg0 >= 0) {
        func_8001F044(arg0);
    }
    if (arg1 == 0 || arg1 >= 0x5C) {
        return 0;
    }
    return func_8001F138(arg1);
}

