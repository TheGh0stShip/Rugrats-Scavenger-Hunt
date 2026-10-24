typedef struct { long long align; } Acmd;
typedef struct { short *ptr; int length; } audio_buffer_t;
typedef struct { Acmd *list; int size; void *callback; void *data; } audio_task_t;
typedef struct {
    void (*start)(void);
    void (*wait)(void);
    void (*submit)(audio_task_t *task);
} scheduler_t;

extern Acmd *D_800C4288;
extern audio_buffer_t *D_800C4284;
extern scheduler_t *D_80055FBC;
extern audio_buffer_t *D_80055FC0;
extern char D_800553C0[];
extern void func_80037CF0(void);
extern int func_8004D2A0(void);
extern unsigned int func_8004D290(void);
extern void func_800427C4(void);
extern void func_8004D3E0(short *buffer, int size);
extern int func_80042FD4(unsigned int samples);
extern void *func_800495D0(void *address);
extern Acmd *func_80040A40(Acmd *cmdList, int *cmdLen, void *outBuf, int outLen);
extern void func_800432B4(void);
extern Acmd *func_8004381C(Acmd *cmdList, int *cmdLen, void *outBuf, int outLen);

void func_80042D88(void)
{
    audio_task_t task;
    int cmdLen;
    unsigned int frame;
    int status;
    unsigned int samples;
    audio_buffer_t *buffer;
    Acmd *cmdp;

    task.list = D_800C4288;
    task.callback = func_80037CF0;
    task.data = D_800553C0;
    frame = 0;
    D_80055FBC->start();
    while (1) {
        D_80055FBC->wait();
        status = func_8004D2A0();
        samples = func_8004D290() >> 2;
        if (status < 0)
            continue;
        func_800427C4();
        if (D_80055FC0 && cmdLen)
            func_8004D3E0(D_80055FC0->ptr, D_80055FC0->length << 2);
        buffer = &D_800C4284[frame];
        buffer->length = func_80042FD4(samples);
        cmdp = func_80040A40(D_800C4288, &cmdLen, func_800495D0(buffer->ptr), buffer->length);
        func_800432B4();
        cmdp = func_8004381C(cmdp, &cmdLen, func_800495D0(buffer->ptr), buffer->length);
        if (cmdLen) {
            task.size = (cmdp - D_800C4288) * sizeof(Acmd);
            D_80055FBC->submit(&task);
            D_80055FC0 = buffer;
        }
        frame = (frame + 1) % 3;
    }
}
