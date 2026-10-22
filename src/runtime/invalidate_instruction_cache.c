void func_80004C6C(void) {
    __asm__ volatile (
        ".set noreorder\n\t"
        "lui $8, 0x8000\n\t"
        "addiu $9, $8, 0x1FF0\n"
        "1:\n\t"
        "cache 1, 0($8)\n\t"
        "bne $8, $9, 1b\n\t"
        "addiu $8, $8, 0x10\n\t"
        ".set reorder"
        ::: "$8", "$9"
    );
}
