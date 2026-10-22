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

void func_80051268(Scheduler *scheduler, SchedulerClient *client, void *queue)
{
    u32 mask;

    mask = func_80047D60(1);

    client->queue = queue;
    client->next = scheduler->client_list;
    scheduler->client_list = client;

    func_80047D60(mask);
}
