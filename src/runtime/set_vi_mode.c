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

void func_80052580(ViMode *mode) {
    u32 status = func_80048900();

    D_800563C4->mode = mode;
    D_800563C4->state = 1;
    D_800563C4->control = D_800563C4->mode->control;
    func_80048920(status);
}
