typedef unsigned char u8;
typedef unsigned short u16;

typedef struct RecordOwner {
    u8 unused0[0x18];
    u8 *records;
} RecordOwner;

int func_8000CDC8(RecordOwner *owner, int index) {
    u8 *records = owner->records;
    register unsigned int offset __asm__("$2");

    if (records == 0) {
        return 0;
    }
    offset = index * 12;
    offset = (unsigned int)records + offset;
    return *(int *)(*(u8 **)(records + 0x1C) +
                   *(u16 *)(offset + 0x28) * 16 + 0xC);
}
