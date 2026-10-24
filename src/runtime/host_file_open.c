typedef int s32;
typedef unsigned int u32;

extern s32 func_800039F0(void);
extern void func_80003A28(s32 mask);
extern u32 func_800046A8(char *text);
extern void func_80003EB0(u32 destination, char *source, u32 length);
extern u32 func_800046C8(u32 command, u32 arg0, u32 arg1, u32 arg2);

s32 func_8000485C(char *name, u32 mode)
{
    s32 mask = func_800039F0();
    u32 length = func_800046A8(name);
    s32 result;

    func_80003EB0(0xB1FF0000, name, length);
    result = func_800046C8(0x303, length, mode, 0);
    if (result & 0x8000) {
        result |= 0xFFFF0000;
    }
    func_80003A28(mask);
    return result;
}
