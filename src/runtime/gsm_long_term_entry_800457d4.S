.set noreorder
.set noat
.text
.globl func_800457D4
.ent func_800457D4
func_800457D4:
.set gain_table, func_800457D4 + 0xB4
    lh      $t3, 0x166($a0)
    addiu   $at, $a1, -0x28
    bltz    $at, lag_ready
     nop
    addiu   $at, $at, -0x50
    bgtz    $at, lag_ready
     nop
    move    $t3, $a1
lag_ready:
    sh      $t3, 0x166($a0)
    move    $at, $ra
    bltzal  $zero, gain_table
.end func_800457D4
