typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    u8 bytes[0x50];
} ViMode;

typedef struct {
    u16 retrace_type;
    u8 pad_002[0x1E];
    u16 prenmi_type;
    u8 pad_022[0x1E];
    u8 interrupt_queue[0x18];
    u8 interrupt_messages[0x20];
    u8 command_queue[0x18];
    u8 command_messages[0x20];
    u8 thread[0x1B0];
    void *client_list;
    void *audio_list_head;
    void *gfx_list_head;
    void *audio_list_tail;
    void *gfx_list_tail;
    void *current_rsp_task;
    void *current_rdp_task;
    u32 frame_count;
} Scheduler;

extern ViMode D_800563D0[];
extern void func_80047EB0(void *queue, void *messages, int count);
extern void func_800521E0(int priority);
extern void func_80052580(ViMode *mode);
extern void func_80052620(int active);
extern void func_80049030(int event, void *queue, u32 message);
extern void func_80052520(void *queue, u32 message, u32 fields);
extern void func_80051358(void *scheduler);
extern void func_80047EE0(void *thread, int id, void (*entry)(void *),
                          void *argument, void *stack, int priority);
extern void func_80049160(void *thread);

void func_80051120(Scheduler *scheduler, void *stack, int priority,
                   u8 mode, u8 fields)
{
    scheduler->current_rsp_task = 0;
    scheduler->current_rdp_task = 0;
    scheduler->client_list = 0;
    scheduler->frame_count = 0;
    scheduler->audio_list_head = 0;
    scheduler->gfx_list_head = 0;
    scheduler->audio_list_tail = 0;
    scheduler->gfx_list_tail = 0;
    scheduler->retrace_type = 1;
    scheduler->prenmi_type = 4;

    func_80047EB0(scheduler->interrupt_queue,
                  scheduler->interrupt_messages, 8);
    func_80047EB0(scheduler->command_queue, scheduler->command_messages, 8);

    func_800521E0(254);
    func_80052580(&D_800563D0[mode]);
    func_80052620(1);
    func_80049030(4, scheduler->interrupt_queue, 0x29B);
    func_80049030(9, scheduler->interrupt_queue, 0x29C);
    func_80049030(14, scheduler->interrupt_queue, 0x29D);
    func_80052520(scheduler->interrupt_queue, 0x29A, fields);

    func_80047EE0(scheduler->thread, 4, func_80051358, scheduler,
                  stack, priority);
    func_80049160(scheduler->thread);
}
