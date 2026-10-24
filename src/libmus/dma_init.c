typedef struct dma_buffer_s {
    struct dma_buffer_s *prev;
    struct dma_buffer_s *next;
    unsigned int start_address;
    int last_frame;
    unsigned char *ptr;
} dma_buffer_t;

extern int D_800C4070;
extern dma_buffer_t *D_800C4074;
extern dma_buffer_t *D_800C4078;
extern void *D_800C407C;
extern void *D_800C4080;
extern int D_800C4084;
extern int D_800C4088;
extern char D_800C4090[];
extern int D_800C40A8;
extern int func_8004FEC0(void);
extern void *func_800430AC(int size);
extern void func_80043100(void *address, int value, int size);
extern void func_80047EB0(void *queue, void *messages, int count);
extern void *func_80042894(void *state);

void *func_80042650(int count, int size)
{
    int i;

    D_800C40A8 = func_8004FEC0();
    D_800C407C = func_800430AC(count * 48);
    D_800C4080 = func_800430AC(count * 8);
    D_800C4078 = func_800430AC(count * 20);
    func_80043100(D_800C4078, 0, count * 20);
    for (i = 0; i < count - 1; i++) {
        D_800C4078[i].next = &D_800C4078[i] + 1;
        (&D_800C4078[i])[1].prev = &D_800C4078[i];
        D_800C4078[i].ptr = func_800430AC(size);
        D_800C4078[i].last_frame = -1;
    }
    D_800C4078[i].ptr = func_800430AC(size);
    D_800C4078[i].last_frame = -1;
    D_800C4084 = size;
    D_800C4088 = 0;
    D_800C4070 = 0;
    D_800C4074 = D_800C4078;
    func_80047EB0(D_800C4090, D_800C4080, count * 2);
    return func_80042894;
}
