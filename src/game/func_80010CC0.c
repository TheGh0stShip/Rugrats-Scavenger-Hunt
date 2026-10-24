typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

typedef struct { u8 pad_00[0x68]; } OSPfs_8005;  /* 0x68 bytes, libultra OSPfs */
typedef struct { u8 pad_00[0x18]; } OSMesgQueue_8005;
extern OSPfs_8005 D_80058990[4];
extern OSMesgQueue_8005 D_80058B30;
extern u8 D_80058B9C;

extern s32 func_8004E8D0(OSMesgQueue_8005 *mq, OSPfs_8005 *pfs, s32 channel);
extern s32 func_8001101C(s32 err);

s32 func_80010CC0(s32 channel) {
    s32 ret = (u32)channel >> 31;

    if (ret == 0) {
        do {
            ret = func_8004E8D0(&D_80058B30, &D_80058990[channel], channel);
        } while (ret == 4);
    }
    return func_8001101C(ret);
}
