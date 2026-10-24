typedef struct RuntimeCommand {
    int next;
    int address;
    short type;
    short pad;
    int value;
    int extra;
} RuntimeCommand;

typedef struct RuntimeOwner {
    unsigned char pad[8];
    unsigned char *resource;
} RuntimeOwner;

extern unsigned char *D_80055AF4;
extern RuntimeCommand *func_80040BA8(void);
extern int func_80040CD4(void *source, int address);
extern int func_8003F6E8(unsigned char *object, int mode, void *value);

void func_80040610(RuntimeOwner *owner, int value, void *source)
{
    RuntimeCommand *command;
    int address_base;
    int address_offset;
    int address;

    if (owner->resource == 0) {
        return;
    }
    command = func_80040BA8();
    if (command == 0) {
        return;
    }
    address_base = *(int *)(D_80055AF4 + 0x1C);
    address_offset = *(int *)(owner->resource + 0x88);
    command->type = 11;
    command->value = (short)value;
    address = address_base + address_offset;
    command->address = address;
    command->extra = func_80040CD4(source, address);
    command->next = 0;
    func_8003F6E8(owner->resource, 3, command);
}
