typedef signed short s16;

typedef struct ObjectState {
    char pad0[0x144];
    int index;
    char pad148[8];
    s16 state;
} ObjectState;

extern int func_80005598(void);
extern int func_80006DB0(int, int);

void func_800079B0(ObjectState *object, int amount) {
    int count;

    count = func_80006DB0(object->state, func_80005598() == 0);
    object->index = (object->index + amount) % count;
}
