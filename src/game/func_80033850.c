/* Target: func_80033850  VRAM 0x80033850  ROM 0x34450
 * Size: 0x2C  Pipeline: KMC GCC 2.7.2 -O2 -G0 -mips3 -mgp32 -mfp32
 * Confidence: high */
typedef struct Queue_80059DB0 { unsigned char pad_00[0x18]; } Queue_80059DB0;
extern Queue_80059DB0 *D_80059DA8;
extern Queue_80059DB0 D_80059DB0;
extern void func_80007EF0(Queue_80059DB0 *q);

void func_80033850(void) {
    D_80059DA8 = &D_80059DB0;
    func_80007EF0(&D_80059DB0);
}
