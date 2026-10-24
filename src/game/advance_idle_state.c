typedef struct StateRecord {
    unsigned char pad_00[0xC];
    int enabled;
    unsigned char pad_10[0xB4];
    int state;
    int timer;
} StateRecord;

void func_8001151C(StateRecord *record) {
    if (record->timer != 0) {
        return;
    }
    if (record->state != 1) {
        if (record->state == 3) {
            record->enabled = 1;
        }
    } else {
        record->state = 3;
        record->timer = 20;
    }
}
