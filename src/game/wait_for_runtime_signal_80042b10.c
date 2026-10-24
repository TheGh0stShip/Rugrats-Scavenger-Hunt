typedef signed short s16;

extern unsigned char *D_800C40C4;
extern void func_80048DD0(void *queue, void **message, int flags);

void func_80042B10(void)
{
    void *message;

    do {
        func_80048DD0(D_800C40C4 + 8, &message, 1);
        func_80048DD0(D_800C40C4 + 8, 0, 0);
    } while (*(s16 *)message != 1);
}
