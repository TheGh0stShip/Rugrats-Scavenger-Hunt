typedef signed short s16;
typedef struct ResourceRecord {
    unsigned char pad_00[0x44]; void *key;
    unsigned char pad_48[0x56]; s16 value;
    unsigned char tail[0x9C];
} ResourceRecord;
extern int D_800C3FD4;
extern ResourceRecord *D_800C3FDC;

int func_8003C090(void *key, s16 value)
{
    int matches;
    int i;
    ResourceRecord *record;
    if (key == 0) return 0;
    matches = 0;
    record = D_800C3FDC;
    for (i = 0; i < D_800C3FD4; i++, record++) {
        if (record->key == key) {
            record->value = value;
            matches++;
        }
    }
    return matches;
}
