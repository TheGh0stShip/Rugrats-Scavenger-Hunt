typedef signed short s16;
typedef unsigned short u16;

typedef struct RuntimeResult {
    char pad0[2];
    u16 value;
} RuntimeResult;

typedef struct RuntimeEntry {
    void *owner;
    char pad4[4];
    s16 index;
    char padA[2];
    s16 value0;
    s16 value1;
    s16 value2;
} RuntimeEntry;

extern int func_8000CEEC(RuntimeEntry *, int);
extern RuntimeResult *func_8000CE50(void *, int, int);

void func_8000CE7C(RuntimeEntry *entry, int value) {
    RuntimeResult *result;

    if (func_8000CEEC(entry, entry->index)) {
        entry->value0 = value;
        entry->value1 = value;
        result = func_8000CE50(entry->owner, entry->index, value);
        entry->value2 = result->value;
    } else {
        entry->value0 = value;
        entry->value1 = value;
        entry->value2 = value;
    }
}
