typedef unsigned char u8;
typedef signed int s32;
typedef unsigned int u32;

struct ResourceEntry {
    s32 offset;
    s32 size;
    s32 packedSize;
};

struct ResourceTable {
    s32 count;
    s32 reserved;
    struct ResourceEntry *entries;
};

struct PackHeader {
    u32 magic;
    u8 pad_04[0x10];
    u32 unpackedSize;
};

extern s32 D_80052EB0[];
extern struct ResourceTable D_80058C38;
extern struct ResourceTable D_80058C48;
extern void *D_80052EC8;
extern u32 D_80058C30;
extern u32 D_80001140;

extern void func_80028B58(s32, void *, s32);
extern void *func_8001A2B0(u32);

void func_80012FB0(void) {
    s32 size;
    s32 i;
    u32 maximum;
    struct ResourceEntry *entry;
    struct PackHeader header;

    func_80028B58(D_80052EB0[0], &D_80058C38, 8);
    size = D_80058C38.count * 12;
    D_80058C38.entries = func_8001A2B0(size);
    func_80028B58(D_80052EB0[0] + 8, D_80058C38.entries, size);

    maximum = 0;
    for (i = 0; i < D_80058C38.count; i++) {
        entry = &D_80058C38.entries[i];
        if ((s32)maximum < entry->packedSize) {
            maximum = entry->packedSize;
        } else {
            func_80028B58(D_80052EB0[0] + entry->offset, &header, 0x18);
            if (header.magic == D_80001140 &&
                maximum < header.unpackedSize) {
                maximum = header.unpackedSize;
            }
        }
    }

    D_80052EC8 = func_8001A2B0(maximum);
    D_80058C30 = maximum;
    func_80028B58(D_80052EB0[4], &D_80058C48, 8);
    size = D_80058C48.count * 12;
    D_80058C48.entries = func_8001A2B0(size);
    func_80028B58(D_80052EB0[4] + 8, D_80058C48.entries, size);
}
