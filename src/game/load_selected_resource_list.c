/* func_80030F74: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_800547B4;
extern s32 D_800535E0;
extern s32 D_80052F18;
extern s32 D_800545E4;
extern s32 D_800546F0[];
extern s32 D_80054724[];
extern s32 D_8005475C[];
extern s32 D_80054780[];
extern void func_80019D44(void);
extern void func_8001E0E0(void);
extern void func_8001ECE4(s32, s32);
extern void func_8001E178(s32);
extern void func_8001E204(s32);
extern void func_80012D9C(s32);

void func_80030F74(void) {
    s32 *list;
    s32 id;
    s32 snd;

    func_80019D44();
    func_8001E0E0();
    if (D_800547B4 != -1) {
        D_800535E0 = 1;
    } else if (D_80052F18 == 0) {
        switch (D_800535E0) {
        case 1:
            D_800547B4 = 0;
            break;
        case 6:
            D_800547B4 = 1;
            break;
        }
    } else if (D_80052F18 == 1) {
        D_800547B4 = 2;
    } else if (D_80052F18 == 2) {
        D_800547B4 = 3;
    }
    switch (D_800547B4) {
        case 0:
            id = 0xCF;
            list = D_800546F0;
            snd = 0x202;
            break;
        case 1:
            id = 0xC3;
            list = D_80054724;
            snd = 0x202;
            break;
        case 2:
            id = 0xDA;
            list = D_8005475C;
            snd = 0x204;
            break;
        case 3:
            id = 0xB9;
            list = D_80054780;
            snd = 0x20B;
            break;
        default:
            goto skip;
    }
    func_8001ECE4(snd, 0x18);
skip:
    D_800547B4 = -1;
    while (*list != 0) {
        func_8001E178(*list);
        list++;
    }
    func_8001E204(id);
    D_800545E4 = 2;
    func_80012D9C(0x14);
}
