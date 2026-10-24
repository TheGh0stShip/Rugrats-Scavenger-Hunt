/* Target: func_8000AD14  VRAM 0x8000AD14  ROM 0xB914  Size: 0x74
 * Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;
struct Obj_8000AD14 {
    u8 pad_00[0xE0];
    s32 unkE0;
};

extern void *D_800581A8;
extern void *D_80052A78[5];
extern void func_80019BD4(void *p);
extern void func_8001A3EC(void *obj);

void func_8000AD14(struct Obj_8000AD14 *obj) {
    s32 i;

    if (obj->unkE0 == 0) {
        D_800581A8 = 0;
        for (i = 0; i < 5; i++) {
            func_80019BD4(D_80052A78[i]);
        }
    }
    func_8001A3EC(obj);
}
