/* Target: func_80004458  VRAM 0x80004458  ROM 0x5058
 * Size: 0x250  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: medium */
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern u32 func_80003A90(u32 addr);
extern void func_80003A58(u32 addr, u32 val);
extern void func_80003ACC(u32 n);
extern u32 func_80004CA4(void);
extern void func_80004CB0(u32 val);
extern void func_800040DC(s32 mode);
extern void func_8000428C(void);

extern u32 D_80057408;
extern u8 D_800575B0;
extern u8 D_800575B2;

void func_80004458(void) {
    s32 cont;
    u32 cmd;

    do {
        cont = 1;
        while ((cmd = func_80003A90(0xB1FFFFF0)) == 0) {
            func_80003ACC(1000);
        }
        if (cmd == 2) {
            func_80003A58(0xB1FFFFFC, 0);
            func_80003ACC(1000);
            continue;
        }
        func_80003A58(0xB1FFFFFC, 0x101);
        while (func_80003A90(0xB1FFFFF0) == cmd) {
            func_80003ACC(500);
        }
        if (func_80003A90(0xB1FFFFF0) == 2) {
            func_80003A58(0xB1FFFFFC, 0);
            continue;
        }
        switch (cmd) {
            case 0x10:
                break;
            case 0x11:
                cont = 0;
                break;
            case 0x12:
                D_800575B0 = 0;
                D_800575B2 = 0;
                D_80057408 &= ~1;
                func_80004CB0(func_80004CA4() & ~1);
                break;
            case 0x13:
                D_800575B2 = 0;
                D_800575B0 = 0;
                break;
            case 0x14:
                D_800575B2 = 0xFF;
                break;
            case 0x15:
                D_800575B2 = 0;
                break;
            case 0x16:
                func_80003A58(0xB1FE0000, (D_800575B2 << 24) | (D_800575B0 << 16));
                break;
            case 0x17:
                func_800040DC(2);
                break;
            case 0x18:
                func_800040DC(1);
                break;
            case 0x19:
                func_8000428C();
                break;
        }
        func_80003A58(0xB1FFFFFC, 0);
        while (func_80003A90(0xB1FFFFF0) == 1) {
            func_80003ACC(500);
        }
    } while (cont != 0 || D_800575B2 == 0);
    func_80003A58(0xB1FFFFF0, 0);
}

