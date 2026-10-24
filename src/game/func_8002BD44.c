/* func_8002BD44  VRAM 0x8002BD44  ROM 0x2C944  Size: 0x2AC
 * Compiler: SN Systems GCC 2.8 (-O2 -G0), asn64 with reviewed scheduling normalization
 */
typedef float f32;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

typedef struct { f32 x, y, z; } Vec3f_8002BD44;

struct Ent_800544E0 {
    Vec3f_8002BD44 pos;   /* 0x00 */
    Vec3f_8002BD44 posA;  /* 0x0C */
    Vec3f_8002BD44 posB;  /* 0x18 */
    Vec3f_8002BD44 vel;   /* 0x24 */
    s16 state;            /* 0x30 */
    u8 pad_32[2];
    void *handle;         /* 0x34 */
    s16 counter;          /* 0x38 */
    u8 pad_3A[2];
};



extern struct Ent_800544E0 D_800544E0[2];
extern s16 D_80054558[2];
extern s32 D_80054478[];
extern s32 D_80057D5C;
extern s32 D_800529F8;
extern char D_8005B218[];
extern char D_8000240C[];
extern char D_80002410[];
extern char D_8000235C[], D_80002368[], D_80002374[], D_80002384[], D_80002390[];
extern char D_8000239C[], D_800023A4[], D_800023B4[], D_800023C4[], D_800023D4[];
extern char D_800023E4[], D_800023EC[], D_800023F8[], D_80002408[];
extern f32 D_8000244C, D_80002450, D_80002454, D_80002458;
extern void func_80033144(s32 r, s32 g, s32 b, s32 a);
extern void *func_80032870(s32 id);
extern void func_80032A20(void *spr, s32 x, s32 y);
extern void func_800329C4(void *spr, s32 x, s16 y);
extern int func_8001F588(s32 a);
extern s32 func_8000E154(s16 a, s32 b);
extern void func_800137E8(s32 a);
extern void func_80013744(void);
extern s32 func_8003A57C(char *buf, const char *fmt, ...);
extern void func_800138FC(char *buf, s32 x, s32 y);
extern u8 *func_80019788(int);
extern void func_8000CE7C(void *obj, s32 frame);
extern void func_8002B2E4(void);
extern void func_8001FB84(s32 r, s32 g, s32 b, s32 a);

void func_8002BD44(u8 *arg0, s32 arg1) {
    char *name;

    if (*(s32 *)(arg0 + 0xF4) < D_80057D5C) {
        func_80033144(0xFF, 0xFF, 0xFF, 0x8C);
        func_80032A20(func_80032870(0xC), 0, D_80054558[1]);
        func_8001F588(0);
        switch ((s16)func_8000E154(*(s16 *)(arg0 + 0x150), *(s32 *)(arg0 + 0x144))) {
            case 0:  name = D_8000235C; break;
            case 1:  name = D_80002368; break;
            case 2:  name = D_80002374; break;
            case 3:  name = D_80002384; break;
            case 4:  name = D_80002390; break;
            case 5:  name = D_8000239C; break;
            case 6:  name = D_800023A4; break;
            case 7:  name = D_800023B4; break;
            case 8:  name = D_800023C4; break;
            case 9:  name = D_800023D4; break;
            case 10: name = D_800023E4; break;
            case 11: name = D_800023EC; break;
            case 12: name = D_800023F8; break;
            default: name = D_80002408; break;
        }
        func_800137E8(4);
        func_80013744();
        func_8003A57C(D_8005B218, D_8000240C, name);
        func_800138FC(D_8005B218, D_80054558[0] + 5, D_80054558[1] + 4);
        func_8000CE7C(func_80019788(D_800544E0[1].handle) + 0x2C, D_80054478[arg1] * 6);
        if (arg1 != 0) {
            D_800544E0[1].pos.x = 1000.0f;
        } else {
            D_800544E0[1].pos.x = 10000.0f;
        }
        D_800544E0[1].pos.y = -650.0f;
        D_800544E0[1].pos.z = -6000.0f;
        func_8002B2E4();
        func_800137E8(0x10);
        func_8001FB84(0, 0xFF, 0, 0xFF);
        func_8003A57C(D_8005B218, D_80002410, arg1);
        if (D_800529F8 == 0) {
            func_80033144(0xFF, 0xFF, 0xFF, 0xFF);
            func_800329C4(func_80032870(0x37), 0x1B0, D_80054558[1] + 4);
        }
    }
}
