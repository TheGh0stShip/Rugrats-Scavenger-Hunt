void func_80004C88(void) {
    __asm__ volatile (
        ".set noreorder\n\t"
        "lui $8, 0x8000\n\t"
        "addiu $9, $8, 0x3FE0\n"
        "1:\n\t"
        "cache 0, 0($8)\n\t"
        "bne $8, $9, 1b\n\t"
        "addiu $8, $8, 0x20\n\t"
        ".set reorder"
        ::: "$8", "$9"
    );
}
