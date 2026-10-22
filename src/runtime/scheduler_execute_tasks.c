typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct {
    void *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    int type;
    unsigned char pad_14[0x24];
    void *output_buffer;
    u64 *output_buffer_size;
} SchedulerTask;

typedef struct {
    unsigned char pad_000[0x274];
    SchedulerTask *current_rsp_task;
    SchedulerTask *current_rdp_task;
} Scheduler;

extern char D_800039B0[];
extern char D_800039B4[];
extern void func_8004B870(char *file, char *expression, int line);
extern void func_80047E80(void);
extern void func_80051D40(void *task_list);
extern void func_80051F4C(void *task_list);
extern int func_80051C80(void *buffer, u64 size);

static int dp_busy = 0;
static int dp_count = 0;

void func_800518FC(Scheduler *scheduler, SchedulerTask *sp_task,
                   SchedulerTask *dp_task)
{
    int result;

    if (scheduler->current_rsp_task != 0) {
        func_8004B870(D_800039B0, D_800039B4, 452);
    }

    if (sp_task) {
        if (sp_task->type == 2) {
            func_80047E80();
        }

        sp_task->state &= ~(0x10 | 0x20);
        func_80051D40(&sp_task->type);
        func_80051F4C(&sp_task->type);
        scheduler->current_rsp_task = sp_task;
        if (sp_task == dp_task) {
            scheduler->current_rdp_task = dp_task;
        }
    }

    if (dp_task && (dp_task != sp_task)) {
        if (!dp_task->output_buffer) {
            func_8004B870(D_800039B0, D_800039B4, 471);
        }

        result = func_80051C80(dp_task->output_buffer,
                               *dp_task->output_buffer_size);

        dp_busy = 1;
        dp_count = 0;

        if (result != 0) {
            func_8004B870(D_800039B0, D_800039B4, 483);
        }

        scheduler->current_rdp_task = dp_task;
    }
}
