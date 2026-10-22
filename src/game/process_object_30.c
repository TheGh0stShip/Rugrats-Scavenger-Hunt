typedef unsigned char u8;

extern void func_8000D768(void *);
extern void func_8001A3EC(void *);

void func_8000D910(u8 *object) {
    func_8000D768(*(void **)(object + 0x30));
    func_8001A3EC(object);
}
