typedef unsigned int u32;

struct ResourceEntry {
    u32 offset;
    u32 size;
    u32 flags;
};

struct ResourceTable {
    u32 count;
    u32 unused;
    struct ResourceEntry *entries;
};

extern struct ResourceTable D_80058C48;
extern unsigned char *D_80052EC0;

void *func_8001361C(u32 index) {
    void *result;

    if (index < D_80058C48.count) {
        result = D_80052EC0 + D_80058C48.entries[index].offset;
    } else {
        result = 0;
    }
    return result;
}
