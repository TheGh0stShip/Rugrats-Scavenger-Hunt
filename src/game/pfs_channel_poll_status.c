typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;
typedef struct { u8 pad_00[0x68]; } OSPfs_8005;
extern OSPfs_8005 D_80058990[4];
extern s32 func_8004E710(OSPfs_8005 *pfs);
extern s32 func_8001101C(s32 err);

void func_80010D50(s32 channel) {
    s32 ret = (u32)channel >> 31;

    if (ret == 0) {
        do {
            ret = func_8004E710(&D_80058990[channel]);
        } while (ret == 4);
    }
    func_8001101C(ret);
}
