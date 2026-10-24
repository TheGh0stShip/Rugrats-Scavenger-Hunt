typedef signed short s16;

typedef struct ObjectState {
    char pad0[0x118];
    unsigned int flags;
    char pad11C[0x34];
    s16 state;
} ObjectState;

extern ObjectState *func_800206A8(void);
extern void func_800211C0(ObjectState *, int);
extern void func_8001EED0(void);

void func_800245FC(ObjectState *object) {
    ObjectState *other;

    object->flags &= ~2;
    other = func_800206A8();
    other->flags &= ~2;
    other = func_800206A8();
    func_800211C0(other, object->state);
    func_8001EED0();
}
