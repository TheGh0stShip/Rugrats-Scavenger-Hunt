typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

extern u32 D_80099B30;
extern u32 D_80099B34;
extern u32 D_80099B08;
extern u32 D_8009BB48;
extern u32 D_8007782C;

void func_80027F48(void) {
    u32 cur;
    u32 diff;
    u32 q;

    D_8009BB48 = cur = D_80099B34 - D_80099B30 * 3;
    diff = cur - D_80099B08;
    if (diff >= 3) {
        q = diff / 3;
        D_8007782C = q;
        if (q >= 5) {
            D_8007782C = 4;
        }
        D_80099B08 = cur - (diff - q * 3);
    } else {
        D_8007782C = 0;
    }
}
