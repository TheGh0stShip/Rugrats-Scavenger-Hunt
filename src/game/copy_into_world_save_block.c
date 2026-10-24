/* func_8000E630: copy a 0x148-byte block into D_800586A8. The empty asm copies the pointer argument as retail does. */
typedef struct { int w[0x52]; } Blk;
extern Blk D_800586A8;
void func_8000E630(Blk *src) { Blk *q; __asm__("" : "=r"(q) : "0"(src)); D_800586A8 = *q; }
