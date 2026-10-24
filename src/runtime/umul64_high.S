.set noreorder
.set noat
.text

.globl func_8003B034
.ent func_8003B034
func_8003B034:
    multu   $a1, $a2
    mflo    $t0
    nop
    nop
    multu   $a0, $a3
    mflo    $v1
    addu    $t0, $t0, $v1
    nop
    multu   $a0, $a2
    mflo    $v1
    mfhi    $v0
    jr      $ra
     addu   $v0, $v0, $t0
.end func_8003B034
