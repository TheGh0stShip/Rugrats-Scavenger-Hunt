/* func_80027FD4  VRAM 0x80027FD4  ROM 0x28BD4  size 0x750
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
} Gwords_80027FD4;

typedef union {
    Gwords_80027FD4 words;
    long long force_structure_alignment;
} Gfx_80027FD4;

struct Frame_8009C358 {
    u8 pad_0000[0x60];
    u64 unk60;
    u8 pad_0068[0xE790 - 0x68];
};

extern u8 D_80077850[];
extern s32 D_80054344;
extern s32 D_80054340;
extern s32 D_80054358;
extern s32 D_80054360;
extern s32 D_8009BB4C;
extern s32 D_80097868;
extern u64 D_80097870;
extern u64 D_80056040;
extern u32 D_80099B34;
extern u32 D_80099B30;
extern u64 D_80097B00;
extern Gfx_80027FD4 *D_8007784C;
extern struct Frame_8009C358 D_8009C358[2];
extern int func_80048DD0(void *, void *, s32);
extern u64 func_80048A10(void);
extern void func_8001116C(void);
extern void func_80013BC8(struct Frame_8009C358 *);
extern void func_80013EE0(struct Frame_8009C358 *);
extern void func_80047E80(void);
extern void func_8001E5F0(void);
extern void func_80019B40(void);
extern void func_80010DC0(void);
extern void func_80027F48(void);
extern void func_8001FF34(void);

void func_80027FD4(void) {
    s16 *msg = 0;
    u32 count = 3;
    s32 side = 0;
    u32 target;
    u32 us;
    Gfx_80027FD4 *g;

    while (D_80054344 >= 2) {
        func_80048DD0(D_80077850, &msg, 1);
        switch (*msg) {
            case 1:
                D_8009BB4C++;
                D_80097868++;
                us = (func_80048A10() - D_80097870) * 1000000 / D_80056040;
                D_80099B34 = (us * 3 + 25000) / 50000;
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

    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xE7000000;
        _g->words.w1 = 0;
    }
    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xB900031D;
        _g->words.w1 = 0x00504240;
    }
    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xFCFFFFFF;
        _g->words.w1 = 0xFFFDF6FB;
    }
    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xFA000000;
        _g->words.w1 = 0xFF;
    }
    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xF68003C0;
        _g->words.w1 = 0;
    }
    {
        Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
        _g->words.w0 = 0xE7000000;
        _g->words.w1 = 0;
    }
    func_80013EE0(&D_8009C358[D_80054340]);
    func_80047E80();
    D_8009BB4C = 0;
    D_80054344++;
    D_80054340 ^= 1;
    func_8001E5F0();
    target = ++D_80099B30;

    while (D_80099B30 <= target + 2) {
        func_80048DD0(D_80077850, &msg, 1);
        switch (*msg) {
            case 1:
                D_8009BB4C++;
                D_80097868++;
                us = (func_80048A10() - D_80097870) * 1000000 / D_80056040;
                D_80099B34 = (us * 3 + 25000) / 50000;
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
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xE7000000;
                    _g->words.w1 = 0;
                }
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xB900031D;
                    _g->words.w1 = 0x00504240;
                }
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xFCFFFFFF;
                    _g->words.w1 = 0xFFFDF6FB;
                }
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xFA000000;
                    _g->words.w1 = 0xFF;
                }
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xF68003C0;
                    _g->words.w1 = 0;
                }
                {
                    Gfx_80027FD4 *_g = (Gfx_80027FD4 *)(D_8007784C++);
                    _g->words.w0 = 0xE7000000;
                    _g->words.w1 = 0;
                }
                func_80013EE0(&D_8009C358[D_80054340]);
                D_8009BB4C = 0;
                D_80054344++;
                D_80054340 ^= 1;
                func_8001E5F0();
                D_80099B30++;
                func_80019B40();
                func_80010DC0();
                func_80027F48();
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

    while (D_80054344 > 0) {
        func_80048DD0(D_80077850, &msg, 1);
        switch (*msg) {
            case 5:
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

    D_80054360 = 1;
    func_80019B40();
    func_80019B40();
    func_80019B40();
    func_8001FF34();
}
