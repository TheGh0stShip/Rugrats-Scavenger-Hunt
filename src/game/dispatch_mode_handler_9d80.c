extern unsigned int func_8000551C(void);
extern void func_80009DF8(int); extern void func_80009FD8(int); extern void func_8000A488(int);
void func_80009D80(int a) {
    switch (func_8000551C()) {
    case 0: func_80009DF8(a); break;
    case 1: func_80009FD8(a); break;
    case 2: func_8000A488(a); break;
    }
}
