/* Target: func_80013358  VRAM 0x80013358  ROM 0x13F58
 * Size: 0x1EC  Compiler: SN GCC 2.8 (-O2)
 * Confidence: medium
 * Pipeline: kmc -O2 -G0 -mips3 -mgp32 -mfp32 */
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef float f32;

struct Hdr_80013358 {
    u8 b[0x18];
};

extern s32 D_80052F18;
extern s32 D_80053410;
extern s32 D_80053414[2];
extern void *D_8005341C[2];
extern u8 D_80053428[];
extern s32 D_8005A990[];
extern void *D_80052EC8;

extern void func_8001330C(s32, s32, s32, void *);
extern void *func_8001A2B0(s32);
extern void func_800166F0(void *, void *, s32 *);

void *func_80013358(s32 id) {
    void *p;
    s32 cur;
    struct Hdr_80013358 hdr;
    s32 outSize;

    p = 0;
    if (D_8005341C[0] != 0 && (u32)(id - 100) < 24 && (D_80053428 - 100)[id] == 0) {
        if (D_80052F18 == 2 ||
            (id != 0x68 && id != 0x6E && id != 0x74 && id != 0x7A &&
             id != 0x69 && id != 0x6F && id != 0x75 && id != 0x7B)) {
            cur = D_80053410;
            if (D_80053414[cur] != id && *(cur ? &D_80053414[0] : &D_80053414[1]) != id) {
                D_80053410 = cur = (cur == 0);
                if (D_80053414[cur] != 0) {
                    D_8005A990[D_80053414[cur]] = 0;
                }
                D_80053414[D_80053410] = id;
                p = D_8005341C[D_80053410];
            }
        }
    }
    func_8001330C(id, 0, 0x18, &hdr);
    func_8001330C(id, 0x18, ((s32 *)&hdr)[3], D_80052EC8);
    if (p == 0) {
        p = func_8001A2B0(((s32 *)&hdr)[2]);
    }
    *(struct Hdr_80013358 *)p = hdr;
    func_800166F0(D_80052EC8, (u8 *)p + 0x18, &outSize);
    return p;
}
