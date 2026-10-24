.set noreorder
.set noat
.text
.globl func_800456E0
.ent func_800456E0
func_800456E0:
.set rpe_table, func_800456E0 + 0xEC
.set large_xmax, func_800456E0 + 0x3C
    addiu   $at, $a0, -7
    bgtz    $at, large_xmax
     nop
    move    $at, $ra
    bltzal  $zero, rpe_table
.end func_800456E0
