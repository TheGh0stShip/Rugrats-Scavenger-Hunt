typedef unsigned char u8;
typedef unsigned int u32;

typedef struct ResourceRecord {
    u32 offset;
    u32 unpacked_size;
    u32 stored_size;
} ResourceRecord;

typedef struct ResourceTable {
    u32 count;
    u32 unused;
    ResourceRecord *records;
} ResourceTable;

extern ResourceTable D_80058C38;
extern u32 D_80052EB0[];
extern u8 *D_80052EC8;
extern void *func_8001A2B0(u32 size);
extern void func_80028B58(u32 rom, void *dst, u32 size);
extern void func_800166F0(u8 *src, u8 *dst, u32 *size);

void *func_80013238(u32 index) {
    u8 *result;
    u32 decoded_size;
    u32 rom;
    u32 unpacked_size;
    u32 stored_size;
    u32 offset;

    if (index >= D_80058C38.count) {
        return 0;
    }
    unpacked_size = D_80058C38.records[index].unpacked_size;
    offset = D_80058C38.records[index].offset;
    rom = D_80052EB0[0] + offset;
    stored_size = D_80058C38.records[index].stored_size;
    result = func_8001A2B0(unpacked_size);
    if (result == 0) {
        return 0;
    }
    if (stored_size != 0) {
        goto compressed;
    }
    func_80028B58(rom, result, unpacked_size);
    goto done;

compressed:
    func_80028B58(rom, D_80052EC8, stored_size);
    func_800166F0(D_80052EC8, result, &decoded_size);

done:
    return result;
}
