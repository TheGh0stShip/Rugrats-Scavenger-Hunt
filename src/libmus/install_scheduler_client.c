typedef struct SchedulerClient {
    char client[8];
    char frame_queue[0x18];
    void *frame_messages[4];
    char task_queue[0x18];
    void *task_messages[4];
} SchedulerClient;

extern void *D_800C40C0;
extern SchedulerClient *D_800C40C4;

extern void *func_800430AC(int size);
extern void func_80047EB0(void *queue, void **messages, int count);
extern void func_80051268(void *scheduler, void *client, void *queue);

void func_80042AAC(void) {
    D_800C40C4 = func_800430AC(sizeof(SchedulerClient));
    func_80047EB0(&D_800C40C4->frame_queue,
                  D_800C40C4->frame_messages, 4);
    func_80047EB0(&D_800C40C4->task_queue,
                  D_800C40C4->task_messages, 4);
    func_80051268(D_800C40C0, D_800C40C4->client,
                  &D_800C40C4->frame_queue);
}
