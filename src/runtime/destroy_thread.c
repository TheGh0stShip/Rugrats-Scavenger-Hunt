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

extern Thread *D_8005606C;
extern Thread *D_80056070;
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern void func_80049280(Thread **queue, Thread *thread);
extern void func_80048774(void);

void func_80048940(Thread *thread)
{
    register u32 saved_mask;
    register Thread *previous;
    register Thread *next;

    saved_mask = func_80048900();

    if (thread == 0) {
        thread = D_80056070;
    } else if (thread->state != 1) {
        func_80049280(thread->queue, thread);
    }

    if (D_8005606C == thread) {
        D_8005606C = D_8005606C->active_next;
    } else {
        previous = D_8005606C;
        next = previous->active_next;
        while (next != 0) {
            if (next == thread) {
                previous->active_next = thread->active_next;
                break;
            }
            previous = next;
            next = previous->active_next;
        }
    }

    if (thread == D_80056070) {
        func_80048774();
    }

    func_80048920(saved_mask);
}
