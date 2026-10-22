typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct RecordOwner {
    u8 unused0[0x18];
    u8 *records;
} RecordOwner;

u32 func_8000CD24(RecordOwner *owner, u32 index, u32 mask) {
    u8 *records = owner->records;
    register u32 result __asm__("$2") = 0;

    if (records != 0) {
        register u32 offset __asm__("$2") = index * 0xC;

        __asm__("" : "=r"(offset) : "0"(offset));
        result = *(u16 *)((u32)records + offset + 0x2A) & mask;
    }
    return result;
}
