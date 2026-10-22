extern void *func_80052160(void);
extern void *func_800521A0(void);

void *func_8005174C(void *task)
{
    void *current;
    void *next;

    if (task) {
        if ((current = func_80052160()) != (next = func_800521A0())) {
            return 0;
        }

        return task;
    }

    return 0;
}
