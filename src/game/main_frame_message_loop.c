/* func_80027AB4  VRAM 0x80027AB4  ROM 0x286B4  size 0x494
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
} Gfx_80027AB4;

struct Frame_8009C358 {
    u8 pad_0000[0x60];
    u64 unk60;
    Gfx_80027AB4 dl[(0xE790 - 0x68) / 8];
};

struct Scene_80053030 {
    void (*unk0)(void);
    void (*init)(void);
};

struct Pad_80058B58 {
    u8 pad_0[0x4];
    u16 button;
    u8 pad_6[0x4];
};

extern s32 D_80059D40;
extern void (*D_80059D98)(void);
extern void (*D_80059D9C)(void);
extern void (*D_80059DA4)(void);
extern struct Scene_80053030 D_80053030[];
extern s32 D_80058C28;
extern s32 D_80052F14;
extern struct Pad_80058B58 D_80058B58[4];
extern u8 D_80077850[];
extern s32 D_8009BB4C;
extern s32 D_80097868;
extern u64 D_80097870;
extern u64 D_80056040;
extern u32 D_80099B34;
extern s32 D_80054344;
extern s32 D_80054340;
extern s32 D_80054358;
extern s32 D_80054360;
extern s32 D_8005434C;
extern s32 D_80099B30;
extern u64 D_80097B00;
extern Gfx_80027AB4 *D_8007784C;
extern struct Frame_8009C358 D_8009C358[2];
extern s32 D_8006D690[2];
extern void func_80028C18(void);
extern s32 func_8003A840(void);
extern void func_800227F0(void);
extern int func_80048DD0(void *, void *, s32);
extern u64 func_80048A10(void);
extern void func_8001116C(void);
extern void func_80013BC8(struct Frame_8009C358 *);
extern void func_8001FF3C(s32);
extern void func_80013EE0(struct Frame_8009C358 *);
extern void func_8001E5F0(void);
extern void func_80019B40(void);
extern void func_80010DC0(void);
extern void func_80027F48(void);
extern void func_8001EDCC(s32);
extern int func_80010CC0(s32);
extern void func_80010FB4(s32);
extern void func_8001F30C(void);
extern void func_8001F064(void);
extern void func_8001F020(s32);

void func_80027AB4(void) {
    s16 *msg = 0;
    s32 pad[4];
    u32 count = 3;
    s32 side = 0;
    s32 prev = -1;
    s32 i;
    s32 n;
    u32 us;

    func_80028C18();
    /* Referencing the exit label keeps the unreachable epilogue that retail
     * emits after the endless frame loop; the asm emits nothing. */
    __asm__("" :: "i"(&&done));
    for (;;) {
        func_8003A840();
        if (D_80059D40 != prev) {
            if (prev != -1) {
                D_80059D98();
            }
            D_80053030[D_80059D40].init();
            prev = D_80059D40;
        }
        if (D_80058C28 == 1 && D_80059D40 == 8 && D_80052F14 < 0) {
            for (i = 0; i < 4; i++) {
                if (D_80058B58[i].button & 0x1000) {
                    D_80052F14 = i;
                    func_800227F0();
                }
            }
        }
        func_80048DD0(D_80077850, &msg, 1);
        switch (*msg) {
            case 1:
                D_8009BB4C++;
                D_80097868++;
                D_80099B34 = ((u32)((func_80048A10() - D_80097870) * 1000000 / D_80056040) * 3 + 25000) / 50000;
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
                D_80059DA4();
                D_80059D9C();
                if (D_80054360 != 0) {
                    D_80054360 = 0;
                    D_80054344 = 0;
                    break;
                }
                func_80013EE0(&D_8009C358[D_80054340]);
                { Gfx_80027AB4 *end = D_8007784C - 0x30; n = end - D_8009C358[D_80054340].dl; }
                if (D_8005434C < n) {
                    D_8005434C = n;
                }
                D_8009BB4C = 0;
                D_80054340 ^= 1;
                D_80054344++;
                func_8001E5F0();
                D_80099B30++;
                func_80019B40();
                func_80010DC0();
                func_80027F48();
                func_8001EDCC(0);
                break;
            case 2:
                D_80097B00 = D_8009C358[side].unk60;
                side ^= 1;
                D_80054344--;
                break;
            case 4:
                for (i = 0; i < 4; i++) {
                    func_80010CC0(i);
                    func_80010FB4(i);
                }
                D_80054344 += 2;
                func_8001F30C();
                func_8001F064();
                func_8001F020(0);
                break;
        }
    }
done:;
}
