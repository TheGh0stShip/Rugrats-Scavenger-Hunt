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
    unsigned char pad_274[0xC];
    int do_audio;
} Scheduler;

extern char D_800039B0[];
extern char D_800039B4[];
extern void func_8004B870(char *file, char *expression, int line);

void func_80051850(Scheduler *scheduler, SchedulerTask *task)
{
    long type = task->type;

    if ((type != 2) && (type != 1)) {
        func_8004B870(D_800039B0, D_800039B4, 412);
    }

    if (type == 2) {
        if (scheduler->audio_list_tail) {
            scheduler->audio_list_tail->next = task;
        } else {
            scheduler->audio_list_head = task;
        }

        scheduler->audio_list_tail = task;
        scheduler->do_audio = 1;
    } else {
        if (scheduler->gfx_list_tail) {
            scheduler->gfx_list_tail->next = task;
        } else {
            scheduler->gfx_list_head = task;
        }

        scheduler->gfx_list_tail = task;
    }

    task->next = 0;
    task->state = task->flags & 3;
}
