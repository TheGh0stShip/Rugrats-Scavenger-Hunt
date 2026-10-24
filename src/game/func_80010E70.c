/* Target: func_80010E70  VRAM 0x80010E70  ROM 0x11A70
 * Size: 0x88  Compiler: KMC GCC 2.7.2 (-O2)
 * Confidence: high
 * Unresolved: func_8004E270 likely osPfsIsPlug */
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

extern s32 func_8004E270(OSMesgQueue_8005 *mq, u8 *pattern);
extern s32 func_80010CC0(s32 channel);
extern s32 func_80010D50(s32 channel);
extern u8 D_80052C04[4];

s32 func_80010E70(s32 channel) {
    s32 ret;

    func_8004E270(&D_80058B30, &D_80058B9C);
    if (!((D_80058B9C >> channel) & 1)) {
        return 0;
    }
    ret = func_80010CC0(channel);
    if (ret != 0) {
        return ret;
    }
    ret = func_80010D50(channel);
    if (ret != 0) {
        return ret;
    }
    D_80052C04[channel] = 1;
    return 0;
}
