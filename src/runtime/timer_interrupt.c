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
extern u32 D_800C4560;
extern u32 func_80048AA0(void);
extern void func_800495C0(u32 count);
extern void func_80049450(u64 time);
extern int func_80048F00(void *queue, void *message, int flags);
extern u64 func_800494B0(Timer *timer);

void func_80049314(void)
{
    Timer *timer;
    u32 count;
    u32 elapsed;

    if (D_80056080->next == D_80056080) {
        return;
    }

    for (;;) {
        timer = D_80056080->next;

        if (timer == D_80056080) {
            func_800495C0(0);
            D_800C4560 = 0;
            break;
        }

        count = func_80048AA0();
        elapsed = count - D_800C4560;
        D_800C4560 = count;

        if (elapsed < timer->value) {
            timer->value -= elapsed;
            func_80049450(timer->value);
            break;
        }

        timer->previous->next = timer->next;
        timer->next->previous = timer->previous;
        timer->next = 0;
        timer->previous = 0;

        if (timer->queue != 0) {
            func_80048F00(timer->queue, timer->message, 0);
        }

        if (timer->interval != 0) {
            timer->value = timer->interval;
            func_800494B0(timer);
        }
    }
}
