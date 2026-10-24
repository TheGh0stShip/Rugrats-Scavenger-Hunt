typedef struct RuntimeCommand {
    int next;
    int address;
    short type;
    short pad;
    int value;
} RuntimeCommand;

typedef struct RuntimeOwner {
    unsigned char pad[8];
    unsigned char *resource;
} RuntimeOwner;

extern unsigned char *D_80055AF4;
extern RuntimeCommand *func_80040BA8(void);
extern int func_8003F6E8(unsigned char *object, int mode, void *value);

void func_800404F0(RuntimeOwner *owner, unsigned int value)
{
    RuntimeCommand *command;
    int address;

    if (owner->resource == 0) {
        return;
    }
    command = func_80040BA8();
    if (command == 0) {
        return;
    }
    address = *(int *)(D_80055AF4 + 0x1C) + *(int *)(owner->resource + 0x88);
    command->type = 12;
    command->value = value & 0xFF;
    command->next = 0;
    command->address = address;
    func_8003F6E8(owner->resource, 3, command);
}
