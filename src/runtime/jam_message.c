typedef unsigned short u16;
typedef unsigned int u32;

typedef struct Thread Thread;
typedef struct MessageQueue MessageQueue;

struct Thread {
    Thread *next;
    int priority;
    Thread **queue;
    Thread *active_next;
    u16 state;
};

struct MessageQueue {
    Thread *waiting_receivers;
    Thread *waiting_senders;
    int valid_count;
    int first;
    int message_count;
    void **messages;
};

extern Thread *D_80056070;
extern u32 func_80048900(void);
extern void func_80048920(u32 mask);
extern void func_80048614(Thread **queue);
extern Thread *func_80048764(Thread **queue);
extern void func_80049160(Thread *thread);

int func_8004FD60(MessageQueue *queue, void *message, int flags)
{
    register u32 saved_mask;
    saved_mask = func_80048900();

    while (queue->valid_count >= queue->message_count) {
        if (flags == 1) {
            D_80056070->state = 8;
            func_80048614(&queue->waiting_senders);
        } else {
            func_80048920(saved_mask);
            return -1;
        }
    }

    queue->first = (queue->first + queue->message_count - 1) % queue->message_count;
    queue->messages[queue->first] = message;
    queue->valid_count++;

    if (queue->waiting_receivers->next != 0) {
        func_80049160(func_80048764(&queue->waiting_receivers));
    }

    func_80048920(saved_mask);
    return 0;
}
