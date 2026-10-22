typedef unsigned char u8;
typedef unsigned short u16;

typedef struct RecordOwner {
    u8 unused0[0x18];
    u8 *records;
} RecordOwner;

int func_8000CD50(RecordOwner *owner, int index, int mask) {
    u8 *records = owner->records;
    u16 *flags;

    if (records != 0) {
        flags = (u16 *)(records + index * 12 + 0x2A);
        *flags |= mask;
        return *flags != 0;
    }
    return 0;
}
