typedef int s32;
typedef float f32;

typedef struct {
    s32 duration;
    f32 value;
    f32 step;
    s32 direction;
    s32 elapsed;
} RuntimeFade;

extern RuntimeFade D_80058C18;

void func_80012D04(s32 duration, s32 direction)
{
    D_80058C18.step = 255 / (duration + 1);
    if (direction == 0) {
        D_80058C18.value = 255.0f;
    } else {
        D_80058C18.value = 0.0f;
    }
    D_80058C18.duration = duration;
    D_80058C18.direction = direction;
    D_80058C18.elapsed = 0;
}
