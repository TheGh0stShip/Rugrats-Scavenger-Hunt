typedef signed short s16;
typedef unsigned char u8;
typedef unsigned int u32;

typedef struct {
    unsigned char pad_0[0x30];
    void *record_list;
    u8 *records;
} RuntimeState;

extern void func_80049794(void *record, u32 arg_2, u32 arg_3);
extern int func_8004C070(u32 *destination, int mode, u32 value);
extern int func_8004B9E0(void *record, int mode, u32 value);

void *func_8004D1F0(RuntimeState *state, int index, u32 arg_2, u32 arg_3) {
    s16 record_index = index;
    u8 *record = state->records + record_index * 0x4C;
    void *payload = record + 0x20;

    func_80049794(payload, arg_2, arg_3);
    func_8004C070(payload, 1, (u32)record);
    func_8004B9E0(state->record_list, 2, (u32)payload);
    return payload;
}
