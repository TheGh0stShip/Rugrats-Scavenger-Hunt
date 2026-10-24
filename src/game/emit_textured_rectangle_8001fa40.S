.set noat
.set noreorder
.text
.globl func_8001FA40
.ent func_8001FA40
func_8001FA40:
    lui     $t5, %hi(D_8006D688)
    lw      $a3, %lo(D_8006D688)($t5)
    move    $t2, $a2
    beqz    $a3, .Lreturn
     li     $v0, -1
    andi    $v0, $a0, 0xFF
    sll     $v0, $v0, 3
    addu    $a2, $a3, $v0
    li      $v0, -1
    lbu     $t3, 0x50($a2)
    lh      $t0, 0x56($a2)
    lhu     $t4, 0x54($a2)
    beq     $t0, $v0, .Lresult
     lui    $v0, %hi(D_8006D684)
    lui     $a0, %hi(D_8006D680)
    sll     $t0, $t0, 2
    lw      $v1, %lo(D_8006D684)($v0)
    lhu     $a3, 0x12($a3)
    sll     $v0, $v1, 3
    subu    $v0, $v0, $v1
    sll     $v0, $v0, 5
    addu    $v0, $v0, $v1
    sll     $v0, $v0, 2
    addu    $v0, $v0, $v1
    sll     $v0, $v0, 5
    lw      $v1, %lo(D_8006D680)($a0)
    lbu     $a0, 0x51($a2)
    addu    $v1, $v1, $v0
    addu    $t0, $t0, $v1
    lbu     $v1, 0x52($a2)
    andi    $a2, $a1, 0xFFFF
    andi    $a1, $t2, 0xFFFF
    lw      $v0, 0($t0)
    addu    $a0, $a0, $a1
    move    $t1, $v0
    addiu   $v0, $v0, 8
    subu    $a3, $a3, $v1
    sw      $v0, 0($t0)
    addu    $v0, $t3, $a2
    sll     $v0, $v0, 2
    andi    $v0, $v0, 0xFFF
    sll     $v0, $v0, 12
    sll     $a3, $a3, 16
    sra     $a3, $a3, 16
    addu    $a0, $a0, $a3
    sll     $a0, $a0, 2
    andi    $a0, $a0, 0xFFF
    lui     $v1, 0xE400
    or      $a0, $a0, $v1
    or      $v0, $v0, $a0
    sll     $a2, $a2, 2
    andi    $a2, $a2, 0xFFF
    sll     $a2, $a2, 12
    addu    $a1, $a1, $a3
    sll     $a1, $a1, 2
    andi    $a1, $a1, 0xFFF
    or      $a2, $a2, $a1
    sw      $v0, 0($t1)
    sw      $a2, 4($t1)
    lw      $v0, 0($t0)
    lui     $a0, 0x0400
    move    $v1, $v0
    addiu   $v0, $v0, 8
    sw      $v0, 0($t0)
    lui     $v0, 0xB400
    sw      $v0, 0($v1)
    sll     $v0, $t4, 21
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
    lw      $v0, %lo(D_8006D688)($t5)
    lw      $v0, 0x14($v0)
    addu    $v0, $t3, $v0
.Lreturn:
    jr      $ra
     nop
.end func_8001FA40
