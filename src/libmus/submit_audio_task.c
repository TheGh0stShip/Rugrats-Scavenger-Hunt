typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
    u32 type;
    u32 flags;
    u64 *ucode_boot;
    u32 ucode_boot_size;
    u64 *ucode;
    u32 ucode_size;
    u64 *ucode_data;
    u32 ucode_data_size;
    u64 *dram_stack;
    u32 dram_stack_size;
    u64 *output_buff;
    u64 *output_buff_size;
    u64 *data_ptr;
    u32 data_size;
    u64 *yield_data_ptr;
    u32 yield_data_size;
} OSTask_t;

typedef struct OSScTask_s {
    struct OSScTask_s *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    OSTask_t list;
    void *msgQ;
    void *msg;
    u32 pad[4];
} OSScTask;

typedef struct {
    u64 *data_ptr;
    u32 data_size;
    u64 *ucode;
    u64 *ucode_data;
} audio_task_t;

typedef struct {
    unsigned char pad[0x30];
    char queue[0x18];
} sched_client_t;

extern void *D_800C40C0;
extern sched_client_t *D_800C40C4;
extern u64 D_800367F0[];
extern u64 D_800368C0[];
extern void *func_80051350(void *sc);
extern int func_80048F00(void *mq, void *msg, int flags);
extern int func_80048DD0(void *mq, void *msg, int flags);

void func_80042B70(audio_task_t *task)
{
    OSScTask t;
    char msg[0x20];

    t.msgQ = D_800C40C4->queue;
    t.next = 0;
    t.msg = msg;
    t.flags = 2;
    t.list.data_ptr = task->data_ptr;
    t.list.data_size = task->data_size;
    t.list.type = 2;
    t.list.ucode_boot = D_800367F0;
    t.list.ucode_boot_size = (u32)D_800368C0 - (u32)D_800367F0;
    t.list.flags = 0;
    t.list.ucode = task->ucode;
    t.list.ucode_data = task->ucode_data;
    t.list.ucode_size = 0x1000;
    t.list.ucode_data_size = 0x800;
    t.list.dram_stack = 0;
    t.list.dram_stack_size = 0;
    t.list.output_buff = 0;
    t.list.output_buff_size = 0;
    t.list.yield_data_ptr = 0;
    t.list.yield_data_size = 0;
    func_80048F00(func_80051350(D_800C40C0), &t, 1);
    func_80048DD0(D_800C40C4->queue, 0, 1);
}
