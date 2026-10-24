extern int D_800C3FF0;
extern void func_8003DC7C(void *object);

int func_8003DFB8(unsigned char *object, unsigned char *definition, int index,
                  int value, int extra_value, int flags)
{
    int identifier;
    int link;
    int data;
    unsigned char *entry;

    func_8003DC7C(object);
    identifier = D_800C3FF0;
    *(short *)(object + 0xA6) = index;
    *(unsigned char **)(object + 0x78) = definition;
    *(short *)(object + 0x9E) = value;
    *(short *)(object + 0xB0) = extra_value;
    *(int *)(object + 0x44) = identifier;
    *(int *)(object + 0x48) = flags;
    link = *(int *)(definition + 0x10);
    D_800C3FF0 = identifier + 1;
    if (link != 0) {
        *(int *)(object + 0x7C) = link;
    }
    entry = definition + index * 8;
    data = *(int *)(entry + 0x18);
    *(int *)(object + 0x80) = data;
    *(int *)(object + 4) = data;
    return *(int *)(object + 0x44);
}
