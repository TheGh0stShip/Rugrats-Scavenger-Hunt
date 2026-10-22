typedef unsigned int u32;

typedef struct SchedulerClient SchedulerClient;

struct SchedulerClient {
    SchedulerClient *next;
    void *queue;
};

typedef struct {
    unsigned char pad[0x260];
    SchedulerClient *client_list;
} Scheduler;

extern u32 func_80047D60(u32 mask);

void func_800512C0(Scheduler *scheduler, SchedulerClient *client)
{
    SchedulerClient *current = scheduler->client_list;
    SchedulerClient *previous = 0;
    u32 mask;

    mask = func_80047D60(1);

    while (current != 0) {
        if (current == client) {
            if (previous) {
                previous->next = client->next;
            } else {
                scheduler->client_list = client->next;
            }
            break;
        }
        previous = current;
        current = current->next;
    }

    func_80047D60(mask);
}
