/* func_8000E680: copy D_800586A8 out, then clear it with func_80038E90. The empty asm copies the pointer argument as retail does. */
typedef struct { int w[0x52]; } Blk;
extern Blk D_800586A8;
extern void func_80038E90(void *, int, int);
void func_8000E680(Blk *dst) { Blk *q; __asm__("" : "=r"(q) : "0"(dst)); *q = D_800586A8; func_80038E90(&D_800586A8, -1, 0x148); }
