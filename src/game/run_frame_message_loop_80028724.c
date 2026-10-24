/* func_80028724  VRAM 0x80028724  ROM 0x29324  size 0x434
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;

typedef struct {
    u32 w0;
    u32 w1;
} Gwords_80028724;

typedef union {
    Gwords_80028724 words;
    long long force_structure_alignment;
} Gfx_80028724;

struct Frame_8009C358 {
    u8 pad_0000[0x60];
    u64 unk60;
    u8 pad_0068[0xE790 - 0x68];
};

extern s32 D_80054344;
extern s32 D_80054340;
extern s32 D_80054358;
extern s32 D_80052F14;
extern s32 D_80099B30;
extern s32 D_8009BB4C;
extern u64 D_80097B00;
extern u8 D_80077850[];
extern Gfx_80028724 *D_8007784C;
extern struct Frame_8009C358 D_8009C358[2];
extern s32 D_8006D690[2];
extern char D_80002130[];
extern int func_80048DD0(void *, void *, s32);
extern void func_80013EE0(struct Frame_8009C358 *);
extern void func_80047E80(void);
extern void func_8001E5F0(void);
extern void func_8001116C(void);
extern void func_80013BC8(struct Frame_8009C358 *);
extern void func_8001FF3C(s32);
extern void func_80013724(void);
extern int func_8001F588(s32);
extern int func_8003A57C(char *, char *, ...);
extern void func_80013AA0(char *, s32, s32);
extern void func_80015740(void);
extern void func_8003376C(void);
extern void func_80013B14(void);
extern void func_80019B40(void);
extern void func_80010DC0(void);

void func_80028724(s32 arg0, s32 arg1) {
    char buf[0x68];
    s16 *msg = 0;
    u32 count = 3;
    s32 side = 0;
    Gfx_80028724 *g;

    while (D_80054344 >= 2) {
        func_80048DD0(D_80077850, &msg, 1);
        __asm__ __volatile__("" :: "i"(&&done));
        switch (*msg) {
            case 1:
                break;
            case 2:
                D_80054344--;
                break;
            case 4:
                D_80054344 += 2;
                break;
        }
    }

    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xE7000000;
        _g->words.w1 = 0;
    }
    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xB900031D;
        _g->words.w1 = 0x00504240;
    }
    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xFCFFFFFF;
        _g->words.w1 = 0xFFFDF6FB;
    }
    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xFA000000;
        _g->words.w1 = 0xFF;
    }
    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xF68003C0;
        _g->words.w1 = 0;
    }
    {
        Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
        _g->words.w0 = 0xE7000000;
        _g->words.w1 = 0;
    }
    func_80013EE0(&D_8009C358[D_80054340]);
    func_80047E80();
    D_8009BB4C = 0;
    D_80054344++;
    D_80054340 ^= 1;
    func_8001E5F0();
    D_80099B30++;

    for (;;) {
        func_80048DD0(D_80077850, &msg, 1);
        __asm__ __volatile__("" :: "i"(&&done));
        switch (*msg) {
            case 1:
                if (D_80054344 >= 2) {
                    break;
                }
                if (++count < 3) {
                    break;
                }
                count = 0;
                func_8001116C();
                D_80054358 = D_80054340;
                func_80013BC8(&D_8009C358[D_80054340]);
                func_8001FF3C(D_8006D690[D_80054340]);
                {
                    Gfx_80028724 *_g = (Gfx_80028724 *)(D_8007784C++);
                    _g->words.w0 = 0xE7000000;
                    _g->words.w1 = 0;
                }
                func_80013724();
                func_8001F588(0);
                func_8003A57C(buf, D_80002130, arg0, arg1);
                func_80013AA0(buf, 0x32, 0x8C);
                func_80015740();
                func_8003376C();
                func_80013B14();
                func_80047E80();
                func_80013EE0(&D_8009C358[D_80054340]);
                D_80054344++;
                D_80054340 ^= 1;
                func_8001E5F0();
                if (D_80052F14 == 0) {
                    D_80099B30++;
                }
                func_80019B40();
                func_80010DC0();
                func_80047E80();
                break;
            case 2:
                D_80097B00 = D_8009C358[side].unk60;
                side ^= 1;
                D_80054344--;
                break;
            case 4:
                D_80054344 += 2;
                break;
        }
    }
done:;
}