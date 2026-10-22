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
extern u64 func_800494B0(Timer *timer);
extern void func_80049450(u64 time);

int func_8004E1D0(Timer *timer, u64 countdown, u64 interval,
                  void *queue, void *message) {
    u64 time;

    timer->next = 0;
    timer->previous = 0;
    timer->interval = interval;
    timer->value = (countdown != 0) ? countdown : interval;
    timer->queue = queue;
    timer->message = message;
    time = func_800494B0(timer);
    if (D_80056080->next == timer) {
        func_80049450(time);
    }
    return 0;
}
