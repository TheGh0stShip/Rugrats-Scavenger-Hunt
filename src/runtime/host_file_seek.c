typedef int s32;
typedef unsigned int u32;

extern s32 func_800039F0(void);
extern void func_80003A28(s32 mask);
extern u32 func_800046C8(u32 command, u32 arg0, u32 arg1, u32 arg2);

u32 func_80004AE0(u32 handle, u32 offset, u32 origin)
{
    s32 mask = func_800039F0();
    u32 result = func_800046C8(0x707, handle, offset, origin);

    func_80003A28(mask);
    return result;
}
