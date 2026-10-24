typedef int s32;
typedef unsigned int u32;

extern s32 func_800039F0(void);
extern void func_80003A28(s32 mask);
extern u32 func_800046C8(u32 command, u32 arg0, u32 arg1, u32 arg2);

s32 func_800048EC(u32 handle)
{
    s32 mask = func_800039F0();
    s32 result = func_800046C8(0x404, handle, 0, 0);

    if (result & 0x8000) {
        result |= 0xFFFF0000;
    }
    func_80003A28(mask);
    return result;
}
