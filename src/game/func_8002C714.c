/* func_8002C714  VRAM 0x8002C714  ROM 0x2D314  Size: 0x1B8
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */
typedef short s16;
typedef int s32;
typedef unsigned char u8;

typedef struct {
    s16 x;
    s16 y;
} Pos_8002C714;

struct Actor_8002C714 {
    u8 pad_000[0xF8];
    s32 unkF8;            /* 0xF8 */
    u8 pad_0FC[0x1E8 - 0xFC];
    s16 unk1E8[4];        /* 0x1E8 */
    s16 unk1F0[4];        /* 0x1F0 */
    s16 unk1F8;           /* 0x1F8 */
};

extern char D_8005B218[];
extern char D_80002410[];
extern void func_80033144(s32 r, s32 g, s32 b, s32 a);
extern void *func_80032870(s32 id);
extern void func_80032A20(void *spr, s32 x, s32 y);
extern int func_8001F588(s32 a);
extern void func_80013724(void);
extern void func_800137E8(s32 a);
extern s32 func_8003A57C(char *D_8005B218, const char *fmt, ...);
extern void func_800138FC(char *D_8005B218, s32 x, s32 y);
extern s32 func_800301C8(struct Actor_8002C714 *a, s32 i);
extern void func_80013790();

void func_8002C714(Pos_8002C714 pos, struct Actor_8002C714 *arg1) {
    s32 i;
    s32 off;
    s32 n;
    s32 k;

    k = arg1->unkF8;
    func_80033144(0xFF, 0xFF, 0xFF, 0xA0);
    i = 0;
    func_80032A20(func_80032870(k + 0x16), pos.x, pos.y);
    func_80032A20(func_80032870(0x1E), pos.x + 0x28, pos.y);
    func_8001F588(i);
    func_80013724();
    func_800137E8(0x10);
    func_8003A57C(D_8005B218, D_80002410, arg1->unk1F8);
    func_800138FC(D_8005B218, pos.x + 0x86, pos.y + 5);
    for (; i < 4; i++) {
        if (arg1->unk1F0[i] != -1) {
            off = (2 - (15 - arg1->unk1F0[i]) / 5) * 0x30;
            func_80032A20(func_80032870(i + 0x1A), pos.x + off, pos.y + 0x18);
            n = arg1->unk1E8[i];
            if (n < 0) {
                n = 0;
            }
            func_8003A57C(D_8005B218, D_80002410, n);
            if (func_800301C8(arg1, i) != 0) {
                func_80013790();
            }
            func_800138FC(D_8005B218, off + pos.x + 0x2A, pos.y + 0x1D);
            func_80013724();
        }
    }
}
