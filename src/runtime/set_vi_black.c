typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    u16 state;
} ViContext;

extern ViContext *D_800563C4;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

void func_80052620(u8 active) {
    u32 status = func_80048900();

    if (active) {
        D_800563C4->state |= 0x20;
    } else {
        D_800563C4->state &= ~0x20;
    }
    func_80048920(status);
}
