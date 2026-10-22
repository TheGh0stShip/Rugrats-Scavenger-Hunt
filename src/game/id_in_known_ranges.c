typedef signed short s16;

extern s16 func_8000DF94(s16);
extern s16 func_8000DFA4(s16);

int func_8000DF48(s16 id) {
    if (func_8000DF94(id)) return 1;
    return func_8000DFA4(id) != 0;
}
