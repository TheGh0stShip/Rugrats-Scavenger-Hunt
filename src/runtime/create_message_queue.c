typedef struct Thread Thread;
typedef struct MessageQueue MessageQueue;

struct MessageQueue {
    Thread *waiting_receivers;
    Thread *waiting_senders;
    int valid_count;
    int first;
    int message_count;
    void **messages;
};

extern Thread D_80056060;

void func_80047EB0(MessageQueue *queue, void **messages, int message_count)
{
    queue->waiting_receivers = &D_80056060;
    queue->waiting_senders = &D_80056060;
    queue->valid_count = 0;
    queue->first = 0;
    queue->message_count = message_count;
    queue->messages = messages;
}
