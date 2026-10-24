typedef struct AudioState {
    char reserved[0x34];
    void *auxiliary_bus;
} AudioState;

extern int D_80055F90;
extern void *D_80055F94[];
extern AudioState *D_800C4040;

extern void func_80042270(void *effect);
extern void func_8004C070(void *filter, int parameter, void *value);

int func_800425F0(int type) {
    char *auxiliary_bus;

    if (type >= D_80055F90) {
        return 1;
    }
    func_80042270(D_80055F94[type]);
    auxiliary_bus = D_800C4040->auxiliary_bus;
    func_8004C070(*(void **)(auxiliary_bus + 0x20), 1, auxiliary_bus);
    return 0;
}
