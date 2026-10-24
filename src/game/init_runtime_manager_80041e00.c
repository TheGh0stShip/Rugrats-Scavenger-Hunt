extern void *D_80055AF0;
extern void *D_80055AF4;
extern void *D_800C4040;
extern void func_80042550(void *config);
extern void func_80041E68(void *config);

void func_80041E00(void *manager, void *config)
{
    if (D_80055AF0 == 0) {
        D_80055AF0 = manager;
        if (D_80055AF4 == 0) {
            func_80042550(config);
            D_800C4040 = D_80055AF0;
            D_80055AF4 = D_80055AF0;
            func_80041E68(config);
        }
    }
}
