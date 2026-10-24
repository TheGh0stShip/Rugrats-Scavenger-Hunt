typedef signed short s16;

typedef struct RuntimeFade {
    s16 step;
    s16 value;
    s16 duration;
    s16 direction;
    s16 elapsed;
} RuntimeFade;

extern RuntimeFade D_80057EE8;

void func_80008F50(int duration, int direction) {
    D_80057EE8.step = 255 / duration;
    if (direction == 0) {
        D_80057EE8.value = 255;
    } else {
        D_80057EE8.value = 0;
    }
    D_80057EE8.duration = duration;
    D_80057EE8.direction = direction;
    D_80057EE8.elapsed = 0;
}
