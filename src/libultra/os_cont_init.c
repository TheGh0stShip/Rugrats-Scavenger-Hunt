typedef signed int s32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef void *OSMesg;

typedef struct OSMesgQueue {
    void *waiting_receivers;
    void *waiting_senders;
    s32 valid_count;
    s32 first;
    s32 message_count;
    OSMesg *messages;
} OSMesgQueue;

typedef struct OSTimer {
    struct OSTimer *next;
    struct OSTimer *previous;
    u64 interval;
    u64 value;
    OSMesgQueue *queue;
    OSMesg message;
} OSTimer;

typedef struct OSContStatus {
    u16 type;
    u8 status;
    u8 error;
} OSContStatus;

#define USEC_TO_CYCLES(value) (((u64)(value) * 46875000ULL) / 1000000ULL)

extern u32 controller_initialized;
extern u8 controller_count;
extern u8 last_controller_command;
extern u8 controller_command_buffer[];
extern OSMesgQueue controller_queue;
extern OSMesg controller_messages[];
extern u64 func_80048A10(void);
extern void func_80047EB0(OSMesgQueue *queue, OSMesg *messages, s32 count);
extern void func_8004E1D0(OSTimer *timer, u64 value, u64 interval,
                          OSMesgQueue *queue, OSMesg message);
extern s32 func_80048DD0(OSMesgQueue *queue, OSMesg *message, s32 flag);
extern void func_8004E110(u8 command);
extern s32 func_8004DD10(s32 direction, void *address);
extern void func_8004E05C(u8 *pattern, OSContStatus *status);
extern void func_8004DDD0(void);

s32 func_8004DEC0(OSMesgQueue *queue, u8 *pattern, OSContStatus *status)
{
    OSMesg message;
    s32 result = 0;
    u64 time;
    OSTimer timer;
    OSMesgQueue timer_queue;

    if (controller_initialized) {
        return 0;
    }
    controller_initialized = 1;
    time = func_80048A10();
    if (time < USEC_TO_CYCLES(500000)) {
        func_80047EB0(&timer_queue, &message, 1);
        func_8004E1D0(&timer, USEC_TO_CYCLES(500000) - time, 0,
                      &timer_queue, &message);
        func_80048DD0(&timer_queue, &message, 1);
    }
    controller_count = 4;
    func_8004E110(0);
    result = func_8004DD10(1, controller_command_buffer);
    func_80048DD0(queue, &message, 1);
    result = func_8004DD10(0, controller_command_buffer);
    func_80048DD0(queue, &message, 1);
    func_8004E05C(pattern, status);
    last_controller_command = 0;
    func_8004DDD0();
    func_80047EB0(&controller_queue, controller_messages, 1);
    return result;
}
