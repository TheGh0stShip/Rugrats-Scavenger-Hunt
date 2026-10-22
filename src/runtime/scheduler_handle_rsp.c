typedef unsigned int u32;

typedef struct SchedulerTask SchedulerTask;

struct SchedulerTask {
    SchedulerTask *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    int type;
};

typedef struct {
    unsigned char pad_000[0x268];
    SchedulerTask *gfx_list_head;
    unsigned char pad_26c[4];
    SchedulerTask *gfx_list_tail;
    SchedulerTask *current_rsp_task;
    SchedulerTask *current_rdp_task;
} Scheduler;

extern char D_800039B0[];
extern char D_800039B4[];
extern void func_8004B870(char *file, char *expression, int line);
extern int func_80052100(void *task_list);
extern int func_800517A0(Scheduler *scheduler, SchedulerTask *task);
extern int func_80051A68(Scheduler *scheduler, SchedulerTask **sp_task,
                         SchedulerTask **dp_task, int state);
extern void func_800518FC(Scheduler *scheduler, SchedulerTask *sp_task,
                          SchedulerTask *dp_task);

void func_8005155C(Scheduler *scheduler)
{
    SchedulerTask *task;
    SchedulerTask *sp_task = 0;
    SchedulerTask *dp_task = 0;
    int state;

    if (!scheduler->current_rsp_task) {
        func_8004B870(D_800039B0, D_800039B4, 272);
    }

    task = scheduler->current_rsp_task;
    scheduler->current_rsp_task = 0;

    if ((task->state & 0x10) && func_80052100(&task->type)) {
        task->state |= 0x20;
        if ((task->flags & 7) == 3) {
            task->next = scheduler->gfx_list_head;
            scheduler->gfx_list_head = task;
            if (scheduler->gfx_list_tail == 0) {
                scheduler->gfx_list_tail = task;
            }
        }
    } else {
        task->state &= ~2;
        func_800517A0(scheduler, task);
    }

    state = (scheduler->current_rsp_task == 0) << 1;
    if (scheduler->current_rdp_task == 0) {
        state |= 1;
    }
    if (func_80051A68(scheduler, &sp_task, &dp_task, state) != state) {
        func_800518FC(scheduler, sp_task, dp_task);
    }
}
