typedef unsigned short u16;
typedef unsigned int u32;
typedef float f32;

typedef struct OSViFieldRegs {
    u32 origin;
    u32 y_scale;
    u32 v_start;
    u32 v_burst;
    u32 v_intr;
} OSViFieldRegs;

typedef struct OSViMode {
    u32 type;
    u32 control;
    u32 width;
    u32 burst;
    u32 v_sync;
    u32 h_sync;
    u32 leap;
    u32 h_start;
    u32 x_scale;
    u32 v_current;
    OSViFieldRegs field[2];
} OSViMode;

typedef struct OSViContext {
    u16 state;
    u16 retrace_count;
    void *framebuffer;
    OSViMode *mode;
    u32 features;
    void *message_queue;
    void *message;
    u32 unused_18;
    u32 unused_1c;
    u32 x_scale;
    f32 y_scale_factor;
    u16 y_offset;
    u16 padding_2a;
    u32 y_scale;
} OSViContext;

extern OSViContext *vi_next;
extern OSViContext *vi_current;
extern u32 func_800495D0(void *address);

void func_8004D720(void)
{
    register OSViMode *mode;
    register OSViContext *context;
    u32 origin;
    u32 horizontal_start;
    u32 scale;
    u32 field;

    field = 0;
    context = vi_next;
    mode = context->mode;
    field = *(volatile u32 *)0xA4400010 & 1;
    origin = func_800495D0(context->framebuffer) + mode->field[field].origin;
    if (context->state & 2) {
        context->x_scale |= mode->x_scale & ~0xFFF;
    } else {
        context->x_scale = mode->x_scale;
    }
    if (context->state & 4) {
        scale = mode->field[field].y_scale & 0xFFF;
        context->y_scale = context->y_scale_factor * scale;
        context->y_scale |= mode->field[field].y_scale & ~0xFFF;
    } else {
        context->y_scale = mode->field[field].y_scale;
    }
    horizontal_start = mode->h_start;
    if (context->state & 0x20) {
        horizontal_start = 0;
    }
    if (context->state & 0x40) {
        context->y_scale = 0;
        origin = func_800495D0(context->framebuffer);
    }
    if (context->state & 0x80) {
        context->y_scale = (context->y_offset << 16) & 0x03FF0000;
        origin = func_800495D0(context->framebuffer);
    }
    *(volatile u32 *)0xA4400004 = origin;
    *(volatile u32 *)0xA4400008 = mode->width;
    *(volatile u32 *)0xA4400014 = mode->burst;
    *(volatile u32 *)0xA4400018 = mode->v_sync;
    *(volatile u32 *)0xA440001C = mode->h_sync;
    *(volatile u32 *)0xA4400020 = mode->leap;
    *(volatile u32 *)0xA4400024 = horizontal_start;
    *(volatile u32 *)0xA4400028 = mode->field[field].v_start;
    *(volatile u32 *)0xA440002C = mode->field[field].v_burst;
    *(volatile u32 *)0xA440000C = mode->field[field].v_intr;
    *(volatile u32 *)0xA4400030 = context->x_scale;
    *(volatile u32 *)0xA4400034 = context->y_scale;
    *(volatile u32 *)0xA4400000 = context->features;
    vi_next = vi_current;
    vi_current = context;
    *vi_next = *vi_current;
}
