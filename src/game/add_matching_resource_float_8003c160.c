typedef struct ResourceRecord {
    unsigned char pad_00[0x30]; float value;
    unsigned char pad_34[0x10]; void *key;
    unsigned char pad_48[0x48]; float source;
    unsigned char tail[0xA8];
} ResourceRecord;
extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C160(void *key, float amount)
{
    int matches;
    int i;
    ResourceRecord *record;
    if (key == 0) return 0;
    matches = 0;
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            record->value = amount + record->source;
            matches++;
        }
    }
    return matches;
}
