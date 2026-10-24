typedef unsigned char u8;
typedef int s32;

typedef struct {
    u8 reserved[0x10];
    s32 identifier;
} PendingRuntimeObject;

extern PendingRuntimeObject *D_800C4008;
extern PendingRuntimeObject *D_800C400C;
extern s32 D_800C3FF8;
extern s32 func_8003E068(PendingRuntimeObject *object, s32 argument,
                         s32 width, s32 height, s32 mode);

s32 func_8003BCA0(s32 argument)
{
    PendingRuntimeObject *object;
    s32 result;

    object = D_800C400C;
    if (object != 0) {
        D_800C400C = 0;
    } else {
        object = D_800C4008;
        if (object == 0) {
            D_800C3FF8 = 0;
            return 0;
        }
    }
    if (D_800C3FF8 == 0) {
        D_800C3FF8 = object->identifier;
    }
    result = func_8003E068(object, argument, 0x80, 0x80, -1);
    D_800C3FF8 = 0;
    return result;
}
