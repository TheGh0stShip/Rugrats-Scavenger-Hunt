.set noreorder
.set noat
.text

.globl func_80000400
.ent func_80000400
func_80000400:
    lui     $sp, 0x803F
    ori     $sp, $sp, 0xFFF0
    lui     $t0, 0x8005
    addiu   $t0, $t0, 0x7280
    lui     $t1, 0x800C
    addiu   $t1, $t1, 0x7E30
    beq     $t0, $t1, 2f
     nop
1:
    addiu   $t0, $t0, 4
    sltu    $at, $t0, $t1
    bnez    $at, 1b
     sw     $zero, -4($t0)
2:
    jal     func_800279B0
     nop
    .word   0x0001008D
.end func_80000400
