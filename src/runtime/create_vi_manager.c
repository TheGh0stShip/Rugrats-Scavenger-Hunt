typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
    int active;
    void *thread;
    void *command_queue;
    void *event_queue;
    void *access_queue;
    void *dma;
    void *edma;
} DeviceManager;

typedef struct {
    u16 type;
    u8 priority;
    u8 pad_03;
    void *return_queue;
    u8 pad_08[0x10];
} IoMessage;

DeviceManager D_80057260 = { 0 };
static u16 retrace_count;
static u8 retrace_padding[6];
static u8 vi_thread[0x1B8];
static u64 vi_thread_stack[0x200] __attribute__((aligned(16)));
static u8 vi_event_queue[0x18] __attribute__((aligned(8)));
static void *vi_event_buffer[5] __attribute__((aligned(8)));
static IoMessage vi_retrace_message __attribute__((aligned(8)));
static IoMessage vi_counter_message __attribute__((aligned(8)));

extern void func_800492C0(void);
extern void func_80047EB0(void *queue, void *messages, int count);
extern void func_80049030(int event, void *queue, void *message);
extern int func_8004FA00(void *thread);
extern void func_80049090(void *thread, int priority);
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern void func_80047EE0(void *thread, int id, void (*entry)(void *),
                          void *argument, void *stack, int priority);
extern void func_8004D610(void);
extern void func_80049160(void *thread);
extern void func_80052378(void *argument);
extern void *func_80052510(void);
extern int func_80048DD0(void *queue, void **message, int flags);
extern int func_80048F00(void *queue, void *message, int flags);
extern void func_8004D720(void);
extern u32 func_80048AA0(void);
extern void func_80049314(void);
extern u32 D_800C4550;
extern u32 D_800C4554;
extern u64 D_800C4558;

void func_800521E0(int priority)
{
    u32 saved_mask;
    int old_priority;
    int current_priority;

    if (D_80057260.active) {
        return;
    }

    func_800492C0();
    func_80047EB0(vi_event_queue, vi_event_buffer, 5);

    vi_retrace_message.type = 13;
    vi_retrace_message.priority = 0;
    vi_retrace_message.return_queue = 0;
    vi_counter_message.type = 14;
    vi_counter_message.priority = 0;
    vi_counter_message.return_queue = 0;

    func_80049030(7, vi_event_queue, &vi_retrace_message);
    func_80049030(3, vi_event_queue, &vi_counter_message);

    old_priority = -1;
    current_priority = func_8004FA00(0);
    if (current_priority < priority) {
        old_priority = current_priority;
        func_80049090(0, priority);
    }

    saved_mask = func_80048900();
    D_80057260.active = 1;
    D_80057260.thread = vi_thread;
    D_80057260.command_queue = vi_event_queue;
    D_80057260.event_queue = vi_event_queue;
    D_80057260.access_queue = 0;
    D_80057260.dma = 0;
    D_80057260.edma = 0;

    func_80047EE0(vi_thread, 0, func_80052378, &D_80057260,
                  vi_thread_stack + 0x200, priority);
    func_8004D610();
    func_80049160(vi_thread);
    func_80048920(saved_mask);

    if (old_priority != -1) {
        func_80049090(0, old_priority);
    }
}

typedef struct {
    u16 state;
    u16 retrace_count;
    u8 pad_04[0xC];
    void *message_queue;
    void *message;
} ViContext;

void func_80052378(void *argument)
{
    ViContext *context;
    DeviceManager *manager;
    IoMessage *message;
    int first;
    u32 count;

    message = 0;
    first = 0;
    context = func_80052510();
    retrace_count = context->retrace_count;
    if (retrace_count == 0) {
        retrace_count = 1;
    }
    manager = argument;

    while (1) {
        func_80048DD0(manager->event_queue, (void **)&message, 1);
        switch (message->type) {
        case 13:
            func_8004D720();
            retrace_count--;

            if (retrace_count == 0) {
                context = func_80052510();
                if (context->message_queue != 0) {
                    func_80048F00(context->message_queue,
                                  context->message, 0);
                }
                retrace_count = context->retrace_count;
            }

            D_800C4554++;

            if (first) {
                count = func_80048AA0();
                D_800C4558 = count;
                first = 0;
            }

            count = D_800C4550;
            D_800C4550 = func_80048AA0();
            count = D_800C4550 - count;
            D_800C4558 = D_800C4558 + count;
            break;

        case 14:
            func_80049314();
            break;

        default:
            break;
        }
    }
}
