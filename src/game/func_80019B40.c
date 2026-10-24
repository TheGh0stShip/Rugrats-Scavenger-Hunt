typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern u8 D_80059F30[];
extern void *D_8005A148[];
extern void func_8001A3EC(void *);
extern void *func_80038E90(void *, s32, s32);

void func_80019B40(void) {
    s32 i;

    for (i = 0; i < 0x211; i++) {
        if (D_80059F30[i] != 0) {
            if (--D_80059F30[i] == 0) {
                func_8001A3EC(D_8005A148[i]);
                func_80038E90(D_8005A148[i], -1, 0x60);
                D_8005A148[i] = 0;
            }
        }
    }
}

