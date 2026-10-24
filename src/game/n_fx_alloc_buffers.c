typedef struct {
    unsigned char pad[0x28];
    void *handler;
} fx_t;

typedef struct {
    unsigned char pad[0x20];
    void *params;
} fx_config_t;

extern void *func_8004CAC0(char *file, int line, void *hp, int num, int size);
extern int func_80042568(void);
extern int func_800425AC(void);
extern void func_80042270(void *params);
extern void func_8004C084(void);
extern void *D_800C4050[6];

void func_80042128(fx_t **out, fx_config_t *c, void *hp)
{
    *out = func_8004CAC0(0, 0, hp, 1, 0x2C);
    (*out)->handler = func_8004C084;
    D_800C4050[0] = func_8004CAC0(0, 0, hp, func_80042568(), 0x28);
    D_800C4050[1] = func_8004CAC0(0, 0, hp, func_800425AC(), 2);
    D_800C4050[2] = func_8004CAC0(0, 0, hp, 1, 0x34);
    D_800C4050[3] = func_8004CAC0(0, 0, hp, 1, 0x20);
    D_800C4050[4] = func_8004CAC0(0, 0, hp, 1, 0x30);
    D_800C4050[5] = func_8004CAC0(0, 0, hp, 1, 8);
    func_80042270(c->params);
}
