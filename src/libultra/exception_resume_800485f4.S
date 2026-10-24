.set noreorder
.set noat
.text

.globl func_800485F4
.ent func_800485F4
func_800485F4:
    nop
    addiu   $t1, $zero, 1
    sw      $t1, 0x18($k0)
    lw      $k1, 0x118($k0)
    lui     $at, 0x2000
    or      $k1, $k1, $at
    .word   0x1000FFB3
     sw     $k1, 0x118($k0)
.end func_800485F4
