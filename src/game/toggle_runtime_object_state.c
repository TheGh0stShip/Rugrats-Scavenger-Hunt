typedef struct ObjectState {
    char pad0[0x1D4];
    int enabled;
} ObjectState;

typedef struct RuntimeState {
    char pad0[0x214];
    int active;
    int value;
} RuntimeState;

extern RuntimeState D_80057F70;
extern int D_80058184;

void func_80009464(ObjectState *object) {
    if (D_80057F70.active != 0) {
        if (!(0 != object->enabled)) {
            object->enabled = 1;
        } else {
            object->enabled = 0;
            D_80057F70.value = 0;
        }
        D_80058184 = 0;
    }
}
