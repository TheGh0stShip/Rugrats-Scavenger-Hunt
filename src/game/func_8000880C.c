typedef signed int s32;
typedef float f32;

extern s32 func_8000857C(f32 m[4][4], f32 x, f32 y, f32 z);

s32 func_8000880C(f32 m[4][4], f32 box[6]) {
    s32 code;

    code = func_8000857C(m, box[0], box[1], box[2]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[0], box[1], box[5]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[0], box[4], box[2]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[0], box[4], box[5]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[3], box[1], box[2]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[3], box[1], box[5]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[3], box[4], box[2]);
    if (code == 0) return 1;
    code &= func_8000857C(m, box[3], box[4], box[5]);
    return code == 0;
}
