typedef struct {
    int pad00;
    int voices;
    int pad08;
    int priority;
    unsigned char pad10[0x28 - 0x10];
    int updates;
    int frequency;
    int buffers;
    int framerate;
    int dma_count;
    int dma_size;
} audio_config_t;

typedef struct {
    int maxVVoices;
    int maxPVoices;
    int maxUpdates;
    int maxFXbusses;
    void *dmaproc;
    void *heap;
    int outputRate;
    unsigned char fxType;
    int pad[2];
} syn_config_t;

typedef struct {
    void *ptr;
    int pad;
} buffer_t;

extern char D_800C4290[];
extern char D_800C40D0[];
extern void *D_800C4288;
extern buffer_t *D_800C4284;
extern void *D_800C4280;
extern void *func_80042650(int count, int size);
extern int func_8004D2B0(int freq);
extern void *func_800430F4(void);
extern void func_80041E00(void *syn, syn_config_t *c);
extern int func_80042F30(int a, int b, int c, int d);
extern void *func_800430AC(int size);
extern void func_80042D88(void *arg);
extern void func_80047EE0(void *t, int id, void (*entry)(void *), void *arg, void *sp, int pri);
extern void func_80049160(void *t);

void func_80042C50(audio_config_t *c, int a1, int fx)
{
    syn_config_t sc;
    unsigned int i;
    int size;
    char *stack;

    sc.maxVVoices = sc.maxPVoices = c->voices;
    sc.maxUpdates = c->updates;
    sc.dmaproc = func_80042650(c->dma_count, c->dma_size);
    sc.fxType = fx;
    sc.outputRate = func_8004D2B0(c->frequency);
    sc.heap = func_800430F4();
    func_80041E00(D_800C4290, &sc);
    size = func_80042F30(c->framerate, sc.outputRate, a1, 15);
    D_800C4288 = func_800430AC(c->buffers * 8);
    D_800C4284 = func_800430AC(0x18);
    for (i = 0; i < 3; i++)
        D_800C4284[i].ptr = func_800430AC(size * 4);
    stack = func_800430AC(0x2000);
    D_800C4280 = stack;
    func_80047EE0(D_800C40D0, 3, func_80042D88, 0, stack + 0x2000, c->priority);
    func_80049160(D_800C40D0);
}
