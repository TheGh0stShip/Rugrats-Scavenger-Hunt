.set noat
.set noreorder
.text
.globl func_8001F850
.ent func_8001F850
func_8001F850:
    move    $t5, $a2
    lui     $t7, %hi(D_8006D688)
    lw      $a3, %lo(D_8006D688)($t7)
    ldc1    $f6, 0x10($sp)
    beqz    $a3, .Lreturn
     li     $v0, -1
    sll     $v0, $t4, 16
    sra     $v0, $v0, 16
    mtc1    $v0, $f2
    cvt.d.w $f2, $f2
    mul.d   $f2, $f2, $f6
    sll     $v0, $v1, 16
    sra     $v0, $v0, 16
    mtc1    $v0, $f0
    cvt.d.w $f0, $f0
    mul.d   $f0, $f0, $f6
    trunc.w.d $f4, $f2
    mfc1    $t2, $f4
    trunc.w.d $f2, $f0
    mfc1    $t3, $f2
    andi    $v0, $a0, 0xFF
    sll     $v0, $v0, 3
    addu    $a2, $a3, $v0
    li      $v0, -1
    lh      $t0, 0x56($a2)
    lhu     $t6, 0x54($a2)
    beq     $t0, $v0, .Lresult
     lui    $v0, %hi(D_8006D684)
    lui     $a0, %hi(D_8006D680)
    sll     $t0, $t0, 2
    lw      $v1, %lo(D_8006D684)($v0)
    lbu     $t4, 0x50($a2)
    lhu     $a3, 0x12($a3)
    sll     $v0, $v1, 3
    subu    $v0, $v0, $v1
    sll     $v0, $v0, 5
    addu    $v0, $v0, $v1
    sll     $v0, $v0, 2
    addu    $v0, $v0, $v1
    sll     $v0, $v0, 5
    lw      $v1, %lo(D_8006D680)($a0)
    lui     $a0, 0xE400
    addu    $v1, $v1, $v0
    addu    $t0, $t0, $v1
    lbu     $v1, 0x52($a2)
    andi    $a2, $a1, 0xFFFF
    lw      $v0, 0($t0)
    andi    $a1, $t5, 0xFFFF
    move    $t1, $v0
    addiu   $v0, $v0, 8
    subu    $a3, $a3, $v1
    sll     $v1, $t2, 16
    sra     $v1, $v1, 16
    addu    $v1, $a2, $v1
    sll     $v1, $v1, 2
    andi    $v1, $v1, 0xFFF
    sll     $v1, $v1, 12
    sw      $v0, 0($t0)
    sll     $v0, $t3, 16
    sra     $v0, $v0, 16
    addu    $v0, $a1, $v0
    sll     $a3, $a3, 16
    sra     $a3, $a3, 16
    addu    $v0, $v0, $a3
    sll     $v0, $v0, 2
    andi    $v0, $v0, 0xFFF
    or      $v0, $v0, $a0
    or      $v1, $v1, $v0
    sll     $a2, $a2, 2
    andi    $a2, $a2, 0xFFF
    sll     $a2, $a2, 12
    addu    $a1, $a1, $a3
    sll     $a1, $a1, 2
    andi    $a1, $a1, 0xFFF
    or      $a2, $a2, $a1
    sw      $v1, 0($t1)
    sw      $a2, 4($t1)
    lw      $v0, 0($t0)
    lui     $a0, 0x0400
    move    $v1, $v0
    addiu   $v0, $v0, 8
    sw      $v0, 0($t0)
    lui     $v0, 0xB400
    sw      $v0, 0($v1)
    sll     $v0, $t6, 21
    sw      $v0, 4($v1)
    lw      $v0, 0($t0)
    ori     $a0, $a0, 0x400
    move    $v1, $v0
    addiu   $v0, $v0, 8
    sw      $v0, 0($t0)
    lui     $v0, 0xB300
    sw      $v0, 0($v1)
    sw      $a0, 4($v1)
.Lresult:
    mtc1    $t4, $f0
    cvt.d.w $f0, $f0
    mul.d   $f0, $f0, $f6
    lw      $v0, %lo(D_8006D688)($t7)
    lwc1    $f2, 0x14($v0)
    cvt.d.w $f2, $f2
    add.d   $f0, $f0, $f2
    trunc.w.d $f2, $f0
    mfc1    $v0, $f2
.Lreturn:
    jr      $ra
     nop
.end func_8001F850
