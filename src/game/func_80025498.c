typedef signed int s32;

extern s32 *func_8002A628(s32);
extern void func_8002A5B0(s32);
extern void func_8002A5EC(s32);

void func_80025498(void *arg0, s32 item) {
    switch (item) {
        case 26: case 27: case 28: case 29:
            *func_8002A628(0) = 0;
            break;
        case 30: case 31: case 32: case 33:
            *func_8002A628(0) = 1;
            break;
        case 34: case 35: case 36: case 37:
            *func_8002A628(0) = 2;
            break;
        case 38: case 39: case 40: case 41:
            *func_8002A628(0) = 3;
            break;
        case 6: case 7: case 8:
            *func_8002A628(0) = 8;
            break;
    }
    switch (item) {
        case 26: case 30: case 34: case 38:
            *func_8002A628(1) = 1;
            break;
        case 6: case 27: case 31: case 35: case 39:
            *func_8002A628(1) = 2;
            break;
        case 7: case 28: case 32: case 36: case 40:
            *func_8002A628(1) = 3;
            break;
        case 8: case 29: case 33: case 37: case 41:
            *func_8002A628(1) = 4;
            break;
    }
    func_8002A5B0(0x34);
    func_8002A5EC(1);
}
