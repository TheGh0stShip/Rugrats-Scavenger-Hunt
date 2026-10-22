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
extern void func_8004871C(Thread **queue, Thread *thread);
extern Thread *func_80048764(Thread **queue);
extern void func_80048774(void);
extern void func_80048614(Thread **queue);

void func_80049160(Thread *thread)
{
    register u32 saved_mask = func_80048900();

    switch (thread->state) {
    case 8:
        thread->state = 2;
        func_8004871C(&D_80056068, thread);
        break;

    case 1:
        if (thread->queue == 0 || thread->queue == &D_80056068) {
            thread->state = 2;
            func_8004871C(&D_80056068, thread);
        } else {
            thread->state = 8;
            func_8004871C(thread->queue, thread);
            func_8004871C(&D_80056068, func_80048764(thread->queue));
        }
        break;
    }

    if (D_80056070 == 0) {
        func_80048774();
    } else if (D_80056070->priority < D_80056068->priority) {
        D_80056070->state = 2;
        func_80048614(&D_80056068);
    }

    func_80048920(saved_mask);
}
