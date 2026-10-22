typedef unsigned char u8;

typedef struct RecordTable {
    unsigned int count;
    u8 unused4[4];
    u8 *records;
} RecordTable;

extern RecordTable D_80058C38;

int func_800131F8(unsigned int index) {
    int result;

    if (index < D_80058C38.count) {
        result = *(int *)(D_80058C38.records + index * 12 + 4);
    } else {
        result = 0;
    }
    __asm__ volatile ("");
    return result;
}
