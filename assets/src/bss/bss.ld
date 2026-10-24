SECTIONS {
 . = 0x80057280;
 .bss (NOLOAD) : { _bss_start = .; *(.data) *(.bss) *(COMMON) _bss_end = .; }
 /DISCARD/ : { *(.reginfo) *(.mdebug*) *(.comment) *(.pdr) *(.gnu.attributes) *(.MIPS.abiflags) }
}
