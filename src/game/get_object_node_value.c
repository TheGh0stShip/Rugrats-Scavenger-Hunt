typedef signed short s16;

struct ObjectNode {
    char pad[0x34];
    s16 value;
};

extern struct ObjectNode *func_800197C8(void *handle);

s16 func_800055E8(void *object) {
    void *handle = *(void **)((char *)object + 0xE0);
    return func_800197C8(handle)->value;
}
