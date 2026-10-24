typedef unsigned int u32;

extern u32 func_80003A90(u32 addr);
extern void func_80003A58(u32 addr, u32 val);
extern void func_80003ACC(u32 n);

void func_8000428C(void) {
    u32 base;
    u32 count;
    u32 i;

    base = func_80003A90(0xB1FFFFF4) & 0xB1FFFFFC;
    count = func_80003A90(0xB1FFFFF8) & 0x01FFFFFC;
    func_80003A58(0xB1FFFFFC, 0);
    while (func_80003A90(0xB0000010) == 0) {
        func_80003ACC(500);
    }
    for (i = 0; i < count / 4; i++) {
        func_80003A58((base & 0xB07FFFFF) + i * 4,
                      func_80003A90(base + i * 4));
    }
    func_80003ACC(2000);
    func_80003A58(0xB1FFFFF4, 0);
}
