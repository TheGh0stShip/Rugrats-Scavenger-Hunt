__asm__(
    ".set noreorder\n"
    ".globl func_8003682C\n"
    "func_8003682C:\n\t"
    "mfc0 $8, $4\n\t"
    "andi $8, $8, 0x80\n\t"
    "bnez $8, 1f\n\t"
    "nop\n\t"
    "jr $31\n"
    "1:\n\t"
    "mtc0 $0, $7\n\t"
    ".set reorder"
);
