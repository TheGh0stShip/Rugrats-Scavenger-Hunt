typedef struct SchedulerClient SchedulerClient;

struct SchedulerClient {
    SchedulerClient *next;
    void *queue;
};

typedef struct {
    unsigned char retrace_message[0x20];
    unsigned char prenmi_message[0x20];
    unsigned char interrupt_queue[0x220];
    SchedulerClient *client_list;
} Scheduler;

extern int func_80048DD0(void *queue, void **message, int flags);
extern int func_80048F00(void *queue, void *message, int flags);
extern void func_8005145C(Scheduler *scheduler);
extern void func_8005155C(Scheduler *scheduler);
extern void func_8005166C(Scheduler *scheduler);

void func_80051358(void *argument)
{
    void *message;
    Scheduler *scheduler = argument;
    SchedulerClient *client;

    while (1) {
        func_80048DD0(scheduler->interrupt_queue, &message, 1);

        switch ((int)message) {
        case 0x29A:
            func_8005145C(scheduler);
            break;

        case 0x29B:
            func_8005155C(scheduler);
            break;

        case 0x29C:
            func_8005166C(scheduler);
            break;

        case 0x29D:
            for (client = scheduler->client_list; client != 0;
                 client = client->next) {
                func_80048F00(client->queue, scheduler->prenmi_message, 0);
            }
            break;
        }
    }
}
