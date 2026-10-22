typedef unsigned int u32;

typedef struct SchedulerClient SchedulerClient;

struct SchedulerClient {
    SchedulerClient *next;
    void *queue;
};

typedef struct {
    unsigned char retrace_message[0x78];
    unsigned char command_queue[0x1E8];
    SchedulerClient *client_list;
    void *audio_list_head;
    void *gfx_list_head;
    void *audio_list_tail;
    void *gfx_list_tail;
    void *current_rsp_task;
    void *current_rdp_task;
    u32 frame_count;
    int do_audio;
} Scheduler;

extern int func_80048DD0(void *queue, void **message, int flags);
extern int func_80048F00(void *queue, void *message, int flags);
extern void func_80051850(Scheduler *scheduler, void *task);
extern void func_80051A30(Scheduler *scheduler);
extern int func_80051A68(Scheduler *scheduler, void **sp_task,
                         void **dp_task, int state);
extern void func_800518FC(Scheduler *scheduler, void *sp_task, void *dp_task);

void func_8005145C(Scheduler *scheduler)
{
    void *task;
    SchedulerClient *client;
    int state;
    void *sp_task = 0;
    void *dp_task = 0;

    scheduler->frame_count++;

    while (func_80048DD0(scheduler->command_queue, &task, 0) != -1) {
        func_80051850(scheduler, task);
    }

    if (scheduler->do_audio && scheduler->current_rsp_task) {
        func_80051A30(scheduler);
    } else {
        state = (scheduler->current_rsp_task == 0) << 1;
        if (scheduler->current_rdp_task == 0) {
            state |= 1;
        }
        if (func_80051A68(scheduler, &sp_task, &dp_task, state) != state) {
            func_800518FC(scheduler, sp_task, dp_task);
        }
    }

    for (client = scheduler->client_list; client != 0;
         client = client->next) {
        func_80048F00(client->queue, scheduler->retrace_message, 0);
    }
}
