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
    unsigned char pad_000[0x264];
    SchedulerTask *audio_list_head;
    SchedulerTask *gfx_list_head;
    SchedulerTask *audio_list_tail;
    SchedulerTask *gfx_list_tail;
    SchedulerTask *current_rsp_task;
    SchedulerTask *current_rdp_task;
    u32 frame_count;
    int do_audio;
} Scheduler;

extern void *func_80052160(void);
extern void *func_800521A0(void);
extern void func_800520E0(void);

void func_80051A30(Scheduler *scheduler)
{
    if (scheduler->current_rsp_task->type == 1) {
        scheduler->current_rsp_task->state |= 0x10;
        func_800520E0();
    }
}

int func_80051A68(Scheduler *scheduler, SchedulerTask **sp_task,
                   SchedulerTask **dp_task, int available);

extern __inline__ SchedulerTask *task_ready(SchedulerTask *task)
{
    void *current;
    void *next;

    if (task) {
        if ((current = func_80052160()) != (next = func_800521A0())) {
            return 0;
        }

        return task;
    }

    return 0;
}

int func_80051A68(Scheduler *scheduler, SchedulerTask **sp_task,
                   SchedulerTask **dp_task, int available)
{
    int initial_available = available;
    SchedulerTask *gfx = scheduler->gfx_list_head;
    SchedulerTask *audio = scheduler->audio_list_head;

    if (scheduler->do_audio && (available & 2)) {
        if (gfx && (gfx->flags & 0x10)) {
            *sp_task = gfx;
            available &= ~2;
        } else {
            *sp_task = audio;
            available &= ~2;
            scheduler->do_audio = 0;
            scheduler->audio_list_head = scheduler->audio_list_head->next;
            if (scheduler->audio_list_head == 0) {
                scheduler->audio_list_tail = 0;
            }
        }
    } else {
        if (task_ready(gfx)) {
            switch (gfx->flags & 7) {
            case 3:
                if (gfx->state & 0x20) {
                    if (available & 2) {
                        *sp_task = gfx;
                        available &= ~2;

                        if (gfx->state & 1) {
                            *dp_task = gfx;
                            available &= ~1;
                        }

                        scheduler->gfx_list_head =
                            scheduler->gfx_list_head->next;
                        if (scheduler->gfx_list_head == 0) {
                            scheduler->gfx_list_tail = 0;
                        }
                    }
                } else {
                    if (available == 3) {
                        *sp_task = *dp_task = gfx;
                        available &= ~3;
                        scheduler->gfx_list_head =
                            scheduler->gfx_list_head->next;
                        if (scheduler->gfx_list_head == 0) {
                            scheduler->gfx_list_tail = 0;
                        }
                    }
                }
                break;

            case 2:
            case 6:
            case 7:
                if (gfx->state & 2) {
                    if (available & 2) {
                        *sp_task = gfx;
                        available &= ~2;
                    }
                } else if (gfx->state & 1) {
                    if (available & 1) {
                        *dp_task = gfx;
                        available &= ~1;
                        scheduler->gfx_list_head =
                            scheduler->gfx_list_head->next;
                        if (scheduler->gfx_list_head == 0) {
                            scheduler->gfx_list_tail = 0;
                        }
                    }
                }
                break;

            case 1:
            case 4:
            case 5:
            default:
                break;
            }
        }
    }

    if (available != initial_available) {
        available = func_80051A68(scheduler, sp_task, dp_task, available);
    }

    return available;
}
