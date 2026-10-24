/* func_80029A40  VRAM 0x80029A40  ROM 0x2A640  size 0x174
 * SN GCC 2.8 -O2 -G0 -mips3; assembled after the reviewed SN scheduling normalization. */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Obj_800B9290 {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

extern struct Obj_800B9290 D_800B9290;
extern s32 D_80054370;
extern f32 D_80059D48[3];
extern u8 D_80059CF0[];
extern char D_800021A4[];
extern char D_800021A8[];
extern char *D_80052FD0[];
extern void *func_8001E630(void);
extern void func_8000CF10(void *, float, float, float, float, float, float, float);
extern u8 *func_800197C8(s32);
extern void * func_8000BDFC(void *, float, float, float, void *, void *, int);
extern int func_8003A57C(char *, char *, ...);
extern void func_800139F8(char *, s32, s32);
extern void func_8001380C(s32, char *, s32, s32);

void func_80029A40(void) {
    char buf[0x28];
    s32 id;
    void *h;

    id = D_800B9290.unk4 * 2 + 0x1F0;
    h = func_8001E630();
    D_80054370 = (D_80054370 + 10) % 360;
    func_8000CF10(h, 0.0f, (f32)D_80054370, 0.0f, 1.0f, 0.0f, 0.0f, -4000.0f);
    func_8000BDFC(func_800197C8(id) + 0x2C, D_80059D48[0], D_80059D48[1], D_80059D48[2],
                  D_80059CF0, h, 0);
    func_8003A57C(buf, "Go to %s's zone?", D_80052FD0[D_800B9290.unk4]);
    func_800139F8(buf, 0x100, 0xA0);
    func_8001380C(0x38, D_800021A4, 100, 200);
    func_8001380C(0x37, D_800021A8, 300, 200);
}
