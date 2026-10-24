typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct Thread Thread;
typedef struct MessageQueue MessageQueue;
typedef struct DeviceManager DeviceManager;

struct MessageQueue {
    Thread *waiting_receivers;
    Thread *waiting_senders;
    int valid_count;
    int first;
    int message_count;
    void **messages;
};

struct DeviceManager {
    int active;
    Thread *thread;
    MessageQueue *command_queue;
    MessageQueue *event_queue;
    MessageQueue *access_queue;
    int (*dma)(int direction, u32 device_address, void *dram_address, u32 size);
    int (*extended_dma)(void *handle, int direction, u32 device_address,
                        void *dram_address, u32 size);
};

extern DeviceManager D_80057210;
extern int D_80057240;
extern Thread D_800C48D0;
extern u64 D_800C4A80[512];
extern MessageQueue D_800C5A80;
extern void *D_800C5A98;
extern MessageQueue D_800C6BB0;

extern void func_80047EB0(MessageQueue *queue, void *messages, int count);
extern void func_80047EE0(Thread *thread, int id, void (*entry)(void *),
                          void *argument, void *stack, int priority);
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern void func_80049030(int event, MessageQueue *queue, void *message);
extern void func_80049090(Thread *thread, int priority);
extern void func_80049160(Thread *thread);
extern int func_8004FA00(Thread *thread);
extern int func_8004FA20(int direction, u32 device_address,
                         void *dram_address, u32 size);
extern int func_8004FAF0(void *handle, int direction, u32 device_address,
                         void *dram_address, u32 size);
extern void func_80050730(void *argument);
extern void func_80050EB0(void);

void func_8004F870(int priority, MessageQueue *command_queue,
                   void *command_buffer, int command_count)
{
    u32 saved_mask;
    int old_priority;
    int current_priority;

    if (D_80057210.active) {
        return;
    }

    func_80047EB0(command_queue, command_buffer, command_count);
    func_80047EB0(&D_800C5A80, &D_800C5A98, 1);

    if (!D_80057240) {
        func_80050EB0();
    }

    func_80049030(8, &D_800C5A80, (void *)0x22222222);
    old_priority = -1;
    current_priority = func_8004FA00(0);

    if (current_priority < priority) {
        old_priority = current_priority;
        func_80049090(0, priority);
    }

    saved_mask = func_80048900();
    D_80057210.active = 1;
    D_80057210.thread = &D_800C48D0;
    D_80057210.command_queue = command_queue;
    D_80057210.event_queue = &D_800C5A80;
    D_80057210.access_queue = &D_800C6BB0;
    D_80057210.dma = func_8004FA20;
    D_80057210.extended_dma = func_8004FAF0;
    func_80047EE0(&D_800C48D0, 0, func_80050730, &D_80057210,
                  &D_800C4A80[512], priority);
    func_80049160(&D_800C48D0);
    func_80048920(saved_mask);

    if (old_priority != -1) {
        func_80049090(0, old_priority);
    }
}
