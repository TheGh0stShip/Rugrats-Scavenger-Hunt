typedef unsigned char u8;
typedef signed int s32;

extern u8 D_80059F30[];
extern void *D_8005A148[];
extern void *D_8005A990[];

s32 func_80019C50(s32 index) {
    void *resource = D_8005A148[index];
    register s32 result __asm__("$2") = 0;

    if (resource != 0) {
        result = 1;
        D_8005A990[index] = resource;
        D_8005A148[index] = 0;
        D_80059F30[index] = 0;
    }
    return result;
}
