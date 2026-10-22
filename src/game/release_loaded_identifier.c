typedef unsigned short u16;

extern int func_80043554(u16);
extern void func_800434F8(u16);

void func_8001F36C(u16 identifier) {
    if (func_80043554(identifier) != 0) {
        func_800434F8(identifier);
    }
}
