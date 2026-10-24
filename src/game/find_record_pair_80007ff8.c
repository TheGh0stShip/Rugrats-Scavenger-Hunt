typedef unsigned short u16;

typedef struct RecordPair {
    struct RecordPair *next;
    unsigned char pad04[4];
    u16 first;
    u16 second;
} RecordPair;

RecordPair *func_80007FF8(RecordPair *record, u16 first, u16 second)
{
    if (first == 0xFFFF) {
        if (second == 0xFFFF) {
            return record;
        }
        while (record != 0) {
            if (record->second == second) {
                return record;
            }
            record = record->next;
        }
    } else if (second == 0xFFFF) {
        while (record != 0) {
            if (record->first == first) {
                return record;
            }
            record = record->next;
        }
    } else {
        while (record != 0) {
            if (record->first == first && record->second == second) {
                return record;
            }
            record = record->next;
        }
    }
    return 0;
}
