typedef int s32;

extern s32 func_8003BCA0(s32);

s32 func_8001F138(s32 arg0) {
    if (arg0 == 0 || arg0 >= 0x5C) {
        return 0;
    }
    return func_8003BCA0(arg0);
}

