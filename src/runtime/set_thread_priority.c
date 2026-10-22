typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Thread Thread;

struct Thread {
    Thread *next;
    int priority;
    Thread **queue;
    Thread *active_next;
    u16 state;
};

extern Thread *D_80056068;
extern Thread *D_80056070;
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern void func_80049280(Thread **queue, Thread *thread);
extern void func_8004871C(Thread **queue, Thread *thread);
extern void func_80048614(Thread **queue);

void func_80049090(Thread *thread, int priority)
{
    register u32 saved_mask;

    saved_mask = func_80048900();

    if (thread == 0) {
        thread = D_80056070;
    }

    if (thread->priority != priority) {
        thread->priority = priority;

        if (thread != D_80056070 && thread->state != 1) {
            func_80049280(thread->queue, thread);
            func_8004871C(thread->queue, thread);
        }

        if (D_80056070->priority < D_80056068->priority) {
            D_80056070->state = 2;
            func_80048614(&D_80056068);
        }
    }

    func_80048920(saved_mask);
}
