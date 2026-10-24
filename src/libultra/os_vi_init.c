typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct OSViMode {
    u32 type;
    u32 control;
} OSViMode;

typedef struct OSViContext {
    u16 state;
    u16 retrace_count;
    void *framebuffer;
    OSViMode *mode;
    u32 features;
    u8 unused[0x20];
} OSViContext;

extern OSViContext vi_contexts[2];
extern OSViContext *vi_current;
extern OSViContext *vi_next;
extern OSViMode vi_mode_pal;
extern OSViMode vi_mode_mpal;
extern OSViMode vi_mode_ntsc;
extern u32 television_type;
extern void func_80051050(void *destination, u32 size);
extern void func_8004D720(void);

void func_8004D610(void)
{
    func_80051050(vi_contexts, sizeof(vi_contexts));
    vi_current = &vi_contexts[0];
    vi_next = &vi_contexts[1];
    vi_next->retrace_count = 1;
    vi_current->retrace_count = 1;
    vi_next->framebuffer = (void *)0x80000000;
    vi_current->framebuffer = (void *)0x80000000;
    if (television_type == 0) {
        vi_next->mode = &vi_mode_pal;
    } else if (television_type == 2) {
        vi_next->mode = &vi_mode_mpal;
    } else {
        vi_next->mode = &vi_mode_ntsc;
    }
    vi_next->state = 0x20;
    vi_next->features = vi_next->mode->control;
    while (*(volatile u32 *)0xA4400010 > 10) {
    }
    *(volatile u32 *)0xA4400000 = 0;
    func_8004D720();
}
