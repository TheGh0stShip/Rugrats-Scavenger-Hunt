typedef struct Thread Thread;

struct Thread {
    Thread *next;
};

void func_80049280(register Thread **queue, register Thread *thread)
{
    register Thread *previous;
    register Thread *next;

    previous = (Thread *)queue;
    next = previous->next;

    while (next != 0) {
        if (next == thread) {
            previous->next = thread->next;
            return;
        }
        previous = next;
        next = previous->next;
    }
}
