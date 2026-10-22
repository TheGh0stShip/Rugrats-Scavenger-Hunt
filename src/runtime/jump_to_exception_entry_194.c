__asm__(
    ".set noreorder\n"
    ".globl func_80004B78\n"
    "func_80004B78:\n\t"
    "lui $26, 0x8000\n\t"
    "ori $26, $26, 0x0194\n\t"
    "jr $26\n\t"
    "nop\n\t"
    ".set reorder"
);
