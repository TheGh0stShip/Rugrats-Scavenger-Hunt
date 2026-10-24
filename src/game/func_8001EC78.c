typedef int s32;

extern void *D_80061DC8[2];
extern void *D_80061E4C;
extern void func_8003BE38(s32, s32);
extern void *func_8001A2B0(s32);

void func_8001EC78(void) {
    func_8003BE38(2, 0);
    func_8003BE38(1, 0);
    D_80061DC8[0] = func_8001A2B0(0x4E20);
    D_80061DC8[1] = func_8001A2B0(0x4E20);
    D_80061E4C = func_8001A2B0(0x3D090);
}
