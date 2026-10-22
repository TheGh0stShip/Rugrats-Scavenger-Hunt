typedef unsigned char u8;
typedef unsigned int u32;
typedef signed int s32;

extern u32 D_80053414;
extern u32 D_80053418;
extern u32 D_8005341C[];
extern u8 D_80059F30[];
extern u32 D_8005A148[];
extern u32 D_8005A990[];

void func_80019BD4(s32 index) {
    u32 *base = D_8005A990;
    u32 *slot = &base[index];
    u32 resource = *slot;

    if (resource != 0) {
        if (resource == D_8005341C[0]) {
            D_80053414 = 0;
        } else if (resource == D_8005341C[1]) {
            D_80053418 = 0;
        } else {
            D_80059F30[index] = 2;
            D_8005A148[index] = D_8005A990[index];
        }
        *slot = 0;
    }
}
