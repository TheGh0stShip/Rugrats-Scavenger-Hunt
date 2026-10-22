typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    u32 type;
    u32 control;
} ViMode;

typedef struct {
    u16 state;
    u16 retrace_count;
    void *framebuffer;
    ViMode *mode;
    u32 control;
} ViContext;

extern ViContext *D_800563C4;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

void func_8004D4A0(u32 features) {
    u32 status = func_80048900();

    if ((features & 1) != 0) {
        D_800563C4->control |= 8;
    }
    if ((features & 2) != 0) {
        D_800563C4->control &= ~8;
    }
    if ((features & 4) != 0) {
        D_800563C4->control |= 4;
    }
    if ((features & 8) != 0) {
        D_800563C4->control &= ~4;
    }
    if ((features & 0x10) != 0) {
        D_800563C4->control |= 0x10;
    }
    if ((features & 0x20) != 0) {
        D_800563C4->control &= ~0x10;
    }
    if ((features & 0x40) != 0) {
        D_800563C4->control |= 0x10000;
        D_800563C4->control &= ~0x300;
    }
    if ((features & 0x80) != 0) {
        D_800563C4->control &= ~0x10000;
        D_800563C4->control |= D_800563C4->mode->control & 0x300;
    }
    D_800563C4->state |= 8;
    func_80048920(status);
}
