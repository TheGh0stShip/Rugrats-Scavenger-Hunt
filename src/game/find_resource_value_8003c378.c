typedef struct ResourceRecord {
    unsigned char pad_00[0x44];
    void *key;
    unsigned char pad_48[0x34];
    int value;
    unsigned char tail[0xBC];
} ResourceRecord;

extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C378(void *key)
{
    int i;
    ResourceRecord *record;

    if (key == 0) {
        return 0;
    }
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            return record->value;
        }
    }
    return 0;
}
