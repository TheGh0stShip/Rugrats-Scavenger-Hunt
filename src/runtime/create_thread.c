typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;

typedef struct Thread Thread;

struct Thread {
    Thread *next;
    int priority;
    Thread **queue;
    Thread *active_next;
    u16 state;
    u16 flags;
    int id;
    int fp;
    char pad_1C[0x1C];
    s64 context_a0;
    char pad_40[0xB0];
    s64 context_sp;
    char pad_F8[0x8];
    s64 context_ra;
    char pad_108[0x10];
    u32 context_sr;
    u32 context_pc;
    char pad_120[0x8];
    u32 context_rcp;
    u32 context_fpcsr;
};

extern Thread *D_8005606C;
extern void func_800488F0(void);
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);

void func_80047EE0(Thread *thread, int id, void (*entry)(void *), void *argument,
                   void *stack, int priority)
{
    register u32 saved_mask;
    u32 interrupt_mask;

    thread->id = id;
    thread->priority = priority;
    thread->next = 0;
    thread->queue = 0;
    thread->context_pc = (u32)entry;
    thread->context_a0 = (s64)(int)argument;
    thread->context_sp = (s64)(int)stack - 16;
    thread->context_ra = (s64)(int)func_800488F0;
    interrupt_mask = 0x03F0FF01;
    thread->context_sr = (interrupt_mask & 0xFF01) | 2;
    thread->context_rcp = (interrupt_mask & 0x03F00000) >> 20;
    thread->context_fpcsr = 0x01000800;
    thread->fp = 0;
    thread->state = 1;
    thread->flags = 0;

    saved_mask = func_80048900();
    thread->active_next = D_8005606C;
    D_8005606C = thread;
    func_80048920(saved_mask);
}
