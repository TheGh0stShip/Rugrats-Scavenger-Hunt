typedef signed short s16;
typedef signed int s32;

typedef struct RuntimeState {
    unsigned char pad[0x34];
    unsigned char *entries;
} RuntimeState;

extern RuntimeState *D_80055AF4;
extern void func_80042128(void *entry);

s32 func_800420D8(s32 index)
{
    s32 offset = (s16)index * 4;
    s32 address = offset + 0x24;

    func_80042128((void *)((s32)D_80055AF4->entries + address));
    return *(s32 *)(offset + (s32)D_80055AF4->entries + 0x24);
}
