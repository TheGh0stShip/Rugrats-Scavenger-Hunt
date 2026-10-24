typedef struct ResourceRecord {
    unsigned char pad[0x44];
    void *resource;
    unsigned char rest[0xF4];
} ResourceRecord;

extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C038(void *resource)
{
    int index;
    int count;
    ResourceRecord *record;
    int matches;

    if (resource == 0) {
        return 0;
    }
    index = 0;
    count = D_800C3FD4;
    record = D_800C3FDC;
    matches = 0;
    while (index < count) {
        if (record->resource == resource) {
            matches++;
        }
        index++;
        record++;
    }
    return matches;
}
