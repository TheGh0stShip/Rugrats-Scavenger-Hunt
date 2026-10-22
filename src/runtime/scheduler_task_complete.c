typedef unsigned int u32;

typedef struct {
    void *next;
    u32 state;
    u32 flags;
    void *framebuffer;
    int type;
    unsigned char pad_14[0x3C];
    void *message_queue;
    void *message;
} SchedulerTask;

extern char D_800039B0[];
extern char D_800039B4[];
extern void func_8004B870(char *file, char *expression, int line);
extern int func_80048F00(void *queue, void *message, int flags);
extern void func_80052620(int active);
extern void func_800525D0(void *framebuffer);

int func_800517A0(void *scheduler, SchedulerTask *task)
{
    int result;
    static int first_time = 1;

    if ((task->state & 3) == 0) {
        if (!task->message_queue) {
            func_8004B870(D_800039B0, D_800039B4, 376);
        }

        result = func_80048F00(task->message_queue, task->message, 1);

        if (task->type == 1) {
            if ((task->flags & 0x20) && (task->flags & 0x40)) {
                if (first_time) {
                    func_80052620(0);
                    first_time = 0;
                }
                func_800525D0(task->framebuffer);
            }
        }
        return 1;
    }

    return 0;
}
