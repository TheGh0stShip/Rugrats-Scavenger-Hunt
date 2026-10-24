/* func_8002B178  VRAM 0x8002B178  ROM 0x2BD78  Size: 0x16C
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */
typedef int s32;
typedef unsigned char u8;

struct Player_800529F0 {
    u8 pad_000[0x144];
    s32 unk144;           /* 0x144 */
};

extern s32 D_800B9280[2];
extern s32 D_800581D0;
extern s32 D_80057D40;
extern struct Player_800529F0 *D_800529F0;
extern char D_80002308[];
extern char D_80002320[];
extern char D_80002338[];
extern int func_8001F588(s32 a);
extern void func_800139F8(char *s, s32 x, s32 y);
extern s32 func_8002A604(void);
extern s32 func_8001C340(void);
extern s32 func_8003A840(void);
extern void func_80028F80(struct Player_800529F0 *p);
extern u8 *func_800197C8(s32 id);
extern void func_8000CE7C(void *obj, s32 frame);
extern void func_8002A5B0(s32 a);

void func_8002B178(void) {
    char *s;
    func_8001F588(0);
    if (D_800B9280[0] == 0) {
        s = "Moving To Nearest Entry";
    } else {
        s = "Moving To Nearest Key";
    }
    func_800139F8(s, 0x100, 0xA0);
    func_800139F8("Press A To Continue", 0x100, 0xB4);
    if (func_8002A604() == 0 && func_8001C340() != 0) {
        D_800581D0 = (func_8003A840() % 18 + D_800529F0->unk144 + 6) % 24;
        do {
            D_800B9280[1] = func_8003A840() % 4;
        } while (D_800B9280[1] == D_80057D40);
        func_80028F80(D_800529F0);
        func_8000CE7C(func_800197C8(0x1F) + 0x2C, 0);
        func_8002A5B0(0);
    }
}
