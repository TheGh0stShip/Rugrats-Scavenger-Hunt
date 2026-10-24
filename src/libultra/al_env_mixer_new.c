typedef int s32;
typedef short s16;
typedef unsigned short u16;

typedef struct EnvMixer {
    char filter[0x14];
    void *state;
    s16 pan;
    s16 volume;
    s16 current_left;
    s16 current_right;
    s16 dry_amount;
    s16 wet_amount;
    u16 left_rate_low;
    s16 left_rate_high;
    s16 left_target;
    u16 right_rate_low;
    s16 right_rate_high;
    s16 right_target;
    s32 delta;
    s32 segment_end;
    s32 first;
    void *control_list;
    void *control_tail;
    void *sources;
    s32 motion;
} EnvMixer;

extern void func_8004B880(void *filter, void *handler, void *set_param, s32 type);
extern void *func_8004CAC0(s32 unused_0, s32 unused_1, void *heap,
                           s32 count, s32 size);
extern void func_8004AB40(void);
extern void func_8004B08C(void);

void func_80049C2C(EnvMixer *mixer, void *heap) {
    func_8004B880(mixer, func_8004AB40, func_8004B08C, 4);
    mixer->state = func_8004CAC0(0, 0, heap, 1, 0x50);
    mixer->first = 1;
    mixer->motion = 0;
    mixer->volume = 1;
    mixer->left_target = 1;
    mixer->right_target = 1;
    mixer->current_left = 1;
    mixer->current_right = 1;
    mixer->dry_amount = 0;
    mixer->wet_amount = 0;
    mixer->left_rate_high = 1;
    mixer->left_rate_low = 0;
    mixer->left_rate_high = 1;
    mixer->left_rate_low = 0;
    mixer->delta = 0;
    mixer->segment_end = 0;
    mixer->pan = 0;
    mixer->control_list = 0;
    mixer->control_tail = 0;
    mixer->sources = 0;
}
