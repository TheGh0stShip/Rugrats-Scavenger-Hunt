typedef struct Thread Thread;

struct Thread {
    Thread *next;
};

typedef struct {
    Thread *mt_queue;
    Thread *full_queue;
    int valid_count;
    int first;
    int message_count;
    void **messages;
} MessageQueue;

extern MessageQueue *D_800C4510;
extern void *D_800C4514;
extern Thread *D_80056068;

extern Thread *func_80048764(Thread **queue);
extern void func_8004871C(Thread **queue, Thread *thread);

void func_8005067C(void)
{
    MessageQueue *queue = D_800C4510;
    int last;

    if (queue == 0 || queue->valid_count >= queue->message_count) {
        return;
    }

    last = (queue->first + queue->valid_count) % queue->message_count;
    queue->messages[last] = D_800C4514;
    queue->valid_count++;

    if (queue->mt_queue->next != 0) {
        func_8004871C(&D_80056068, func_80048764(&queue->mt_queue));
    }
}
