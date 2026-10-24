typedef unsigned int u32;

extern u32 func_80003A90(u32 address);
extern void func_80003A58(u32 address, u32 value);
extern void func_80003ACC(u32 count);

u32 func_800046C8(u32 command, u32 argument_0, u32 argument_1,
                  u32 argument_2)
{
    func_80003A58(0xB1FFFFE4, argument_0);
    func_80003A58(0xB1FFFFE8, argument_1);
    func_80003A58(0xB1FFFFEC, argument_2);
    func_80003A58(0xB1FFFFE0, command);
    do {
        func_80003ACC(1000);
        if (func_80003A90(0xB1FFFFF0) != 0) {
            __asm__ volatile("break 1");
        }
    } while (func_80003A90(0xB1FFFFE0) != 0);
    return func_80003A90(0xB1FFFFDC);
}
