/* func_8002CC34  VRAM 0x8002CC34  ROM 0x2D834  Size: 0x474
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */
typedef short s16;
typedef int s32;
typedef unsigned char u8;

struct Actor_8002CC34 {
    u8 pad_000[0x1E8];
    s16 unk1E8[4];        /* 0x1E8 */
};

extern s32 D_800B92DC;
extern s32 D_800B92E0;
extern s32 D_800B92E4;
extern char D_8005B218[];
extern char D_80002634[];
extern char D_80002640[];
extern char D_8000264C[];
extern char D_80002658[];
extern char D_80002668[];
extern unsigned int func_8001BF3C(void);
extern s32 func_8003A840(void);
extern s32 *func_8002A628(s32 a);
extern s32 func_800301C8(struct Actor_8002CC34 *a, s32 i);
extern void func_8002A5EC(s32 value);
extern s32 func_8003A57C(char *buf, const char *fmt, ...);
extern void func_800137E8(s32 a);
extern void func_800138FC(char *buf, s32 x, s32 y);
extern int func_8001F138(s32 a);
extern s32 func_8001C2C8(void);
extern void func_8001A5C0(void);

void func_8002CC34(struct Actor_8002CC34 *arg0) {
    switch (func_8001BF3C()) {
        case 0x2E:
            D_800B92E0 = D_800B92DC = (func_8003A840() % 3 + 1) * 2;
            func_8002A5EC(func_800301C8(arg0, *func_8002A628(0)) == 0 ? 0x2F : 0x30);
            D_800B92E4 = 0;
            break;
        case 0x2F:
            if (--D_800B92E4 > 0) {
                switch (*func_8002A628(0)) {
                    case 0:
                        func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002640);
                        break;
                    case 1:
                        func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_8000264C);
                        break;
                    case 2:
                        func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002658);
                        break;
                    case 3:
                        func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002668);
                        break;
                }
                func_800137E8(8);
                func_800138FC(D_8005B218, 0x100, 0xB4);
                break;
            }
            D_800B92E4 = 5;
            switch (*func_8002A628(0)) {
                case 0:
                    arg0->unk1E8[0]++;
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002640);
                    break;
                case 1:
                    arg0->unk1E8[1]++;
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_8000264C);
                    break;
                case 2:
                    arg0->unk1E8[2]++;
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002658);
                    break;
                case 3:
                    arg0->unk1E8[3]++;
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002668);
                    break;
            }
            D_800B92E0--;
            func_8001F138(4);
            func_800137E8(8);
            func_800138FC(D_8005B218, 0x100, 0xB4);
            if (D_800B92E0 == 0 || func_800301C8(arg0, *func_8002A628(0)) != 0) {
                func_8002A5EC(0x30);
            }
            break;
        case 0x30:
            func_800137E8(8);
            switch (*func_8002A628(0)) {
                case 0:
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002640);
                    break;
                case 1:
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_8000264C);
                    break;
                case 2:
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002658);
                    break;
                case 3:
                    func_8003A57C(D_8005B218, D_80002634, D_800B92DC, D_80002668);
                    break;
            }
            func_800138FC(D_8005B218, 0x100, 0xB4);
            if (func_8001C2C8() != 0) {
                func_8001A5C0();
            }
            break;
    }
}
