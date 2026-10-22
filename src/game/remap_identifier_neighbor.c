extern void func_8000DC38(int);

void func_8000695C(int id) {
    switch (id) {
    case 0xF9: id = 0xFA; break;
    case 0xFA: id = 0xF9; break;
    case 0xFB: id = 0xFC; break;
    case 0xFC: id = 0xFB; break;
    default: return;
    }
    func_8000DC38(id);
}
