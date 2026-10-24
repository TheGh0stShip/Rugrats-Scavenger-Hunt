typedef struct RuntimeCommand {
    int next;
    int address;
    short type;
    short pad;
    float value;
} RuntimeCommand;

typedef struct RuntimeOwner {
    unsigned char pad[8];
    unsigned char *resource;
} RuntimeOwner;

extern unsigned char *D_80055AF4;
extern RuntimeCommand *func_80040BA8(void);
extern int func_8003F6E8(unsigned char *object, int mode, void *value);

void func_80040580(RuntimeOwner *owner, float value)
{
    RuntimeCommand *command;
    int address_base;
    int address_offset;

    if (owner->resource == 0) {
        return;
    }
    command = func_80040BA8();
    if (command == 0) {
        return;
    }
    address_base = *(int *)(D_80055AF4 + 0x1C);
    address_offset = *(int *)(owner->resource + 0x88);
    command->type = 7;
    command->value = value;
    command->next = 0;
    command->address = address_base + address_offset;
    func_8003F6E8(owner->resource, 3, command);
}
