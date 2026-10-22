typedef unsigned int u32;

typedef struct {
    void *next;
    u32 state;
    u32 flags;
    u32 pad_0c;
    int type;
} SchedulerTask;

typedef struct {
    unsigned char pad_000[0x274];
    SchedulerTask *current_rsp_task;
    SchedulerTask *current_rdp_task;
} Scheduler;

extern void func_8004B870(char *file, char *expression, int line);
extern char D_800039B0[];
extern char D_800039B4[];
extern void func_800517A0(Scheduler *scheduler, SchedulerTask *task);
extern int func_80051A68(Scheduler *scheduler, SchedulerTask **sp_task,
                         SchedulerTask **dp_task, int state);
extern void func_800518FC(Scheduler *scheduler, SchedulerTask *sp_task,
                          SchedulerTask *dp_task);

void func_8005166C(Scheduler *scheduler)
{
    SchedulerTask *task;
    SchedulerTask *sp_task = 0;
    SchedulerTask *dp_task = 0;
    int state;

    if (!scheduler->current_rdp_task) {
        func_8004B870(D_800039B0, D_800039B4, 317);
    }
    if (scheduler->current_rdp_task->type != 1) {
        func_8004B870(D_800039B0, D_800039B4, 318);
    }

    task = scheduler->current_rdp_task;
    scheduler->current_rdp_task = 0;
    task->state &= ~1;

    func_800517A0(scheduler, task);

    state = (scheduler->current_rsp_task == 0) << 1;
    if (scheduler->current_rdp_task == 0) {
        state |= 1;
    }
    if (func_80051A68(scheduler, &sp_task, &dp_task, state) != state) {
        func_800518FC(scheduler, sp_task, dp_task);
    }
}
