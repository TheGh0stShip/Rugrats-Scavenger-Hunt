typedef unsigned int u32;

typedef struct {
    u32 type;
    u32 flags;
} Task;

extern u32 func_80052150(void);

int func_80052100(Task *task) {
    u32 status = func_80052150();
    int result = (status & 0x100) ? 1 : 0;

    if ((status & 0x80) != 0) {
        task->flags |= result;
        task->flags &= ~2;
    }
    return result;
}
