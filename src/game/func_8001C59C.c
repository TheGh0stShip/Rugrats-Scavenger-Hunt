typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

extern void *D_80059DA8;
extern void * func_80007FF8(void *, unsigned short, unsigned short);
extern s32 func_8001C2C8(void);
extern void *func_80005544(void);
extern s32 func_80005598(void *);
extern void func_8001A5C0(void);

void func_8001C59C(void) {
    if (func_80007FF8(D_80059DA8, 7, 0xFFFF) == 0) {
        if (func_8001C2C8() != 0 || func_80005598(func_80005544()) == 0) {
            func_8001A5C0();
        }
    }
}

