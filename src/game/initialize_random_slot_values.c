typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    u8 reserved[0x18];
    s16 slots[4];
    s16 field_20;
} RuntimeSlotValues;

extern void *func_80038E90(void *destination, s32 value, s32 length);
extern s32 func_8003A840(void);

void func_8002E7FC(RuntimeSlotValues *values, s32 excluded)
{
    s32 index;
    s32 selected;

    if (excluded == 8) {
        values->slots[0] = 15;
        values->slots[1] = 15;
        values->slots[2] = 15;
        values->slots[3] = 15;
        return;
    }
    values->field_20 = 15;
    func_80038E90(values->slots, 0, 8);
    values->slots[excluded] = -1;
    for (index = 0; index < 3; index++) {
        do {
            selected = func_8003A840() % 4;
            if (values->slots[selected] != 0) {
                selected = -1;
            }
        } while (selected < 0);
        values->slots[selected] = (3 - index) * 5;
    }
}
