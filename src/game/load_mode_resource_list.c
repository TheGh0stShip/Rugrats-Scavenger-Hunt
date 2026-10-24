/* func_80030BE8: SN GCC 2.8, scheduling normalized */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

extern s32 D_80052F18;
extern s32 D_800B9344;
extern s32 D_80054644[];
extern s32 D_8005468C[];
extern s32 D_800546C0[];
extern void func_80019D44(void);
extern void func_8001E0E0(void);
extern void func_8001E178(s32);
extern void func_8001E204(s32);
extern void func_8001ECE4(s32, s32);
extern void func_8001E718(s32);

void func_80030BE8(void) {
    s32 *list;
    s32 id;
    s32 snd;

    func_80019D44();
    func_8001E0E0();
    if (D_80052F18 == 0) {
        list = D_80054644;
        id = 0x91;
    } else if (D_80052F18 == 1) {
        list = D_8005468C;
        id = 0xA2;
    } else if (D_80052F18 == 2) {
        list = D_800546C0;
        id = 0xAF;
    }
    while (*list != 0) {
        func_8001E178(*list);
        list++;
    }
    func_8001E204(id);
    D_800B9344 = 7;
    snd = D_80052F18;
    if (snd == 0) {
        snd = 0x202;
    } else if (snd == 1) {
        snd = 0x204;
    } else {
        snd = 0x20B;
    }
    func_8001ECE4(snd, 0x18);
    func_8001E718(0xF);
}
