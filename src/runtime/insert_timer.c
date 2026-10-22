typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct Timer Timer;

struct Timer {
    Timer *next;
    Timer *previous;
    u64 interval;
    u64 value;
    void *queue;
    void *message;
};

extern Timer *D_80056080;
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);

u64 func_800494B0(Timer *timer)
{
    Timer *current;
    u64 time;
    u32 saved_mask = func_80048900();

    current = D_80056080->next;
    time = timer->value;
    for (; current != D_80056080 && time > current->value;
         current = current->next) {
        time -= current->value;
    }

    timer->value = time;

    if (current != D_80056080) {
        current->value -= time;
    }

    timer->next = current;
    timer->previous = current->previous;
    current->previous->next = timer;
    current->previous = timer;
    func_80048920(saved_mask);

    return time;
}
