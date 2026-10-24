.set noat
.set noreorder
.text
.globl func_8003F0A8
.ent func_8003F0A8
func_8003F0A8:
    lui     $v0, %hi(D_80055AF4)
    lw      $v0, %lo(D_80055AF4)($v0)
    addiu   $sp, $sp, -0x20
    sw      $ra, 0x18($sp)
    sw      $s1, 0x14($sp)
    sw      $s0, 0x10($sp)
    lw      $s0, 0x14($v0)
    move    $a2, $a0
    bnez    $s0, .Lhave_voice
     move   $s1, $zero
    lw      $s0, 4($v0)
    beqz    $s0, .Lscan_allocated
     nop
.Lhave_voice:
    move    $a0, $s0
    jal     func_8004CA4C
     sw     $s0, 0($a2)
    lui     $a1, %hi(D_80055AF4)
    lw      $a1, %lo(D_80055AF4)($a1)
    move    $a0, $s0
    jal     func_8004CA2C
     addiu  $a1, $a1, 0xC
    j       .Lreturn
     move   $v0, $s1
.Lscan_allocated:
    lw      $s0, 0xC($v0)
    beqz    $s0, .Lreturn
     move   $v0, $s1
.Lloop:
    lw      $v0, 8($s0)
    lh      $v1, 0x16($v0)
    sll     $v0, $a1, 16
    sra     $v0, $v0, 16
    slt     $v0, $v0, $v1
    bnez    $v0, .Lnext
     nop
    lw      $v0, 0x88($s0)
    bnez    $v0, .Lnext
     nop
    sw      $s0, 0($a2)
    lw      $v0, 8($s0)
    li      $s1, 1
    lhu     $a1, 0x16($v0)
.Lnext:
    lw      $s0, 0($s0)
    bnez    $s0, .Lloop
     move   $v0, $s1
.Lreturn:
    lw      $ra, 0x18($sp)
    lw      $s1, 0x14($sp)
    lw      $s0, 0x10($sp)
    jr      $ra
     addiu  $sp, $sp, 0x20
    nop
    nop
.end func_8003F0A8
