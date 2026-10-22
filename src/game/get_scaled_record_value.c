typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct RecordOwner {
    u8 unused0[0x18];
    u8 *records;
} RecordOwner;

u32 func_8000CE50(RecordOwner *owner, u32 index, u32 scale) {
    register u8 *records __asm__("$3") = owner->records;
    register u32 count __asm__("$4") = *(u16 *)records;
    register u8 *address __asm__("$3");

    address = records + index * 0xC;
    return *(u32 *)(address + 0x24) + scale * count;
}
