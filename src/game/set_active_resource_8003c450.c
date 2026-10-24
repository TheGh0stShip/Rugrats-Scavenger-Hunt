typedef struct ResourceCommand {
    unsigned char type;
    unsigned char pad[3];
    void *resource;
} ResourceCommand;

extern void *D_800C4000;
extern void *D_800C4004;
extern int func_8003C744(const void *entry);

void func_8003C450(void *resource)
{
    int accepted = 1;

    if (resource == 0) {
        ResourceCommand command;
        command.type = 2;
        command.resource = D_800C4004;
        D_800C4004 = command.resource;
        accepted = func_8003C744(&command);
    }
    if (accepted != 0) {
        D_800C4000 = resource;
    }
}
