typedef unsigned int u32;

typedef struct ResourceRecord {
    u32 offset;
    u32 unpacked_size;
    u32 stored_size;
} ResourceRecord;

extern ResourceRecord *D_80058C40;
extern u32 D_80052EB0[];
extern void func_80028B58(u32 rom, void *dst, u32 size);

void func_8001330C(u32 index, u32 offset, u32 size, void *destination) {
    func_80028B58(D_80052EB0[0] + D_80058C40[index].offset + offset,
                  destination, size);
}
