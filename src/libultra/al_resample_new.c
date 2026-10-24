typedef int s32;

typedef struct Resampler {
    char filter[0x14];
    void *state;
    float ratio;
    float delta;
    s32 motion;
    s32 first;
    s32 unity_pitch;
    void *control_list;
    void *control_tail;
} Resampler;

extern void func_8004B880(void *filter, void *handler, void *set_param, s32 type);
extern void *func_8004CAC0(s32 unused_0, s32 unused_1, void *heap,
                           s32 count, s32 size);
extern void func_8004BA10(void);
extern void func_8004BBFC(void);

void func_80049D94(Resampler *resampler, void *heap) {
    func_8004B880(resampler, func_8004BA10, func_8004BBFC, 1);
    resampler->state = func_8004CAC0(0, 0, heap, 1, 0x20);
    resampler->motion = 0;
    resampler->first = 1;
    resampler->control_tail = 0;
    resampler->delta = 0.0f;
    resampler->unity_pitch = 0;
    resampler->control_list = 0;
    resampler->ratio = 1.0f;
}
