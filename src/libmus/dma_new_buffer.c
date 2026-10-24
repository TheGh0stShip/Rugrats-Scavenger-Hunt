typedef struct dma_buffer_s {
    struct dma_buffer_s *prev;
    struct dma_buffer_s *next;
    int last_frame;
    unsigned int start_address;
    unsigned char *ptr;
} dma_buffer_t;

typedef struct {
    unsigned short type;
    unsigned char pri;
    unsigned char status;
    void *retQueue;
    void *dramAddr;
    unsigned int devAddr;
    unsigned int size;
    void *piHandle;
} io_message_t;

extern int D_800C4030;
extern dma_buffer_t *D_800C4070;
extern dma_buffer_t *D_800C4074;
extern io_message_t *D_800C407C;
extern unsigned int D_800C4084;
extern int D_800C4088;
extern char D_800C4090[];
extern void *D_800C40A8;
extern void *D_800C40B0;
extern int func_8004FCC0(void *handle, io_message_t *message, int direction);

dma_buffer_t *func_8004290C(unsigned int addr, int len)
{
    void *handle;
    unsigned int end;
    dma_buffer_t *dp;
    dma_buffer_t *last;
    dma_buffer_t *buf;
    dma_buffer_t *next;
    io_message_t *message;
    int index;

    if ((addr & 0xFF000000) == 0xFF000000) {
        handle = D_800C40B0;
        addr = addr & 0xFFFFFF;
        addr = addr + 0x140000;
    } else {
        if (D_800C4030 & 1)
            return 0;
        handle = D_800C40A8;
    }
    last = 0;
    end = addr + len;
    for (dp = D_800C4070; dp != 0; dp = dp->next) {
        if (addr < dp->start_address)
            break;
        if (end <= dp->start_address + D_800C4084)
            return dp;
        last = dp;
    }
    buf = D_800C4074;
    if (buf == 0)
        return D_800C4070;
    D_800C4074 = buf->next;
    if (last) {
        next = last->next;
        buf->next = next;
        if (next)
            next->prev = buf;
        buf->prev = last;
        last->next = buf;
    } else {
        buf->prev = 0;
        buf->next = D_800C4070;
        if (D_800C4070)
            D_800C4070->prev = buf;
        D_800C4070 = buf;
    }
    index = D_800C4088;
    buf->start_address = addr & ~1;
    buf->last_frame = 2;
    D_800C4088 = index + 1;
    message = &D_800C407C[index];
    message->pri = 0;
    message->retQueue = D_800C4090;
    message->dramAddr = buf->ptr;
    message->devAddr = buf->start_address;
    message->size = D_800C4084;
    func_8004FCC0(handle, message, 0);
    return buf;
}
