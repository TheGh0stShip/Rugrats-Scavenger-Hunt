typedef unsigned char u8;
typedef short s16;
typedef int s32;

typedef struct {
    u8 reserved_00[0x2C];
    float transform[12];
    u8 reserved_5C[0xC8];
    s32 field_124;
    s32 field_128;
    u8 reserved_12C[0x10];
    u8 field_13C;
    u8 field_13D;
    u8 reserved_13E[0x12];
    s16 field_150;
    s16 field_152;
} RuntimeActor;

extern RuntimeActor *func_800206A8(void);
extern s32 func_8001EEAC(void);
extern void func_8001EEBC(void);
extern void func_8001ECE4(s32 identifier, s32 argument);

void func_800210E4(RuntimeActor *source)
{
    RuntimeActor *destination = func_800206A8();

    destination->transform[0] = source->transform[0];
    destination->transform[1] = source->transform[1];
    destination->transform[2] = source->transform[2];
    destination->transform[3] = source->transform[3];
    destination->transform[4] = source->transform[4];
    destination->transform[5] = source->transform[5];
    destination->transform[6] = source->transform[6];
    destination->transform[7] = source->transform[7];
    destination->transform[8] = source->transform[8];
    destination->transform[9] = source->transform[9];
    destination->transform[10] = source->transform[10];
    destination->transform[11] = source->transform[11];
    destination->field_124 = source->field_124;
    destination->field_128 = source->field_128;
    destination->field_13C = source->field_13C;
    destination->field_13D = source->field_13D;
    destination->field_150 = source->field_150;
    destination->field_152 = source->field_152;
    if (func_8001EEAC() != 0x20F) {
        func_8001EEBC();
        func_8001ECE4(0x20F, 0x38);
    }
}
