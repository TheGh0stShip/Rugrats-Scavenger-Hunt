typedef struct RuntimeWorkspace {
    unsigned char first[0x18];
    unsigned char second[0x18];
    unsigned char final[8];
} RuntimeWorkspace;

extern void func_80047EB0(void *first, void *final, int value);
extern void func_80047C50(void *arg0, void *arg1);
extern void func_80050FA0(void *second, int arg1, int arg2, void *arg3,
                          void *arg4, void *arg5, void *first);
extern void func_80048DD0(void *queue, void *message, int flags);
extern void func_80048F00(void *queue, void *message, int flags);

void func_80043CE4(void *arg0, void *arg1, void *arg2, void *arg3)
{
    RuntimeWorkspace workspace;

    func_80047EB0(workspace.first, workspace.final, 1);
    func_80047C50(arg1, arg2);
    func_80050FA0(workspace.second, 0, 0, arg0, arg1, arg2, workspace.first);
    func_80048DD0(workspace.first, 0, 1);
    func_80048F00(arg3, 0, 1);
}
