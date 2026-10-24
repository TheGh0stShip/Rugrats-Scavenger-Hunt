typedef struct ResourceRecord {
    unsigned char pad_00[0x44]; void *key;
    unsigned char pad_48[0x2C]; unsigned char *primary; unsigned char *fallback;
    unsigned char tail[0xC0];
} ResourceRecord;
extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C5DC(void *key)
{
    int i;
    ResourceRecord *record;
    if (key == 0) return 0;
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            if (record->primary != 0) return *(int *)(record->primary + 0x20);
            return *(int *)(record->fallback + 0x14);
        }
    }
    return 0;
}
