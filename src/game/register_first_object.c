typedef void *Ptr;

extern Ptr D_800C3FFC;
extern void func_8003DA50(Ptr);

void func_8003C2E4(Ptr object) {
    func_8003DA50(object);
    if (D_800C3FFC == 0) {
        D_800C3FFC = object;
    }
}
