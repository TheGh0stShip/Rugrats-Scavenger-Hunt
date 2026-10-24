typedef struct ObjectState {
    char pad0[0x118];
    unsigned int flags;
} ObjectState;

extern void func_8001BED8(int);
extern int func_80025FC4(ObjectState *, int);

void func_80024DAC(ObjectState *object) {
    func_8001BED8(0x18);
    if (func_80025FC4(object, 13)) {
        object->flags |= 1;
    }
}
