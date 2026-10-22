typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    u16 state;
    u16 retrace_count;
    void *framebuffer;
} ViContext;

extern ViContext *D_800563C4;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

void func_800525D0(void *framebuffer) {
    u32 status = func_80048900();

    D_800563C4->framebuffer = framebuffer;
    D_800563C4->state |= 0x10;
    func_80048920(status);
}
