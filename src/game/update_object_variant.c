typedef struct ObjectState {
    unsigned char pad[0xE0];
    int variant;
} ObjectState;

extern void func_8000AA1C(void);
extern void func_8000AB74(ObjectState *object);

void func_8000A9E0(ObjectState *object) {
    register ObjectState *current __asm__("$3") = object;

    if (object->variant == 0) {
        func_8000AA1C();
    } else {
        __asm__("" : "=r"(current) : "0"(current));
        func_8000AB74(current);
    }
}
