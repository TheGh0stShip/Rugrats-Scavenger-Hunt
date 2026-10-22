typedef unsigned int u32;
typedef unsigned short u16;

typedef struct {
    u16 state;
    u16 retrace_count;
    void *framebuffer;
    void *mode;
    u32 control;
    void *queue;
    void *message;
} ViContext;

extern ViContext *D_800563C4;
extern u32 func_80048900(void);
extern void func_80048920(u32 status);

void func_80052520(void *queue, void *message, u32 retrace_count) {
    u32 status = func_80048900();

    D_800563C4->queue = queue;
    D_800563C4->message = message;
    D_800563C4->retrace_count = retrace_count;
    func_80048920(status);
}
