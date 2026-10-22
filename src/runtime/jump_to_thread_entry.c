__asm__(
    ".set noreorder\n"
    ".globl func_80047FB0\n"
    "func_80047FB0:\n\t"
    "lui $26, 0x8004\n\t"
    "addiu $26, $26, 0x7FC0\n\t"
    "jr $26\n\t"
    "nop\n\t"
    ".set reorder"
);
