typedef unsigned int u32;
typedef int s32;
typedef double f64;

typedef struct {
    s32 type;
    char *name;
    f64 argument_1;
    f64 argument_2;
    f64 result;
} MathException;

extern s32 D_800C3FB0;
extern s32 func_8003B020(MathException *exception);

f64 func_8003AFB0(s32 type, char *name, f64 argument_1, f64 argument_2,
                  f64 result)
{
    MathException exception;

    exception.type = type;
    exception.name = name;
    exception.argument_1 = argument_1;
    exception.argument_2 = argument_2;
    exception.result = result;
    if (func_8003B020(&exception) == 0) {
        D_800C3FB0 = ((u32)(type - 3) < 2) ? 34 : 33;
    }
    return exception.result;
}
