.set noat
.set noreorder
.text
.globl func_8002D404
.ent func_8002D404
func_8002D404:
    addiu   $sp, $sp, -0x18
    sw      $ra, 0x10($sp)
    jal     func_8001BF3C
     nop
    bnez    $v0, .Lfinish
     lui    $v1, 0x8005
    move    $t0, $zero
    li      $t3, 1
    lui     $v0, 0x800C
    move    $t1, $v0
    lui     $v0, 0x8005
    addiu   $t2, $v0, 0x4490
    lui     $v0, 0x8005
    addiu   $a3, $v0, 0x44E0
    lui     $v0, 0x8005
    sw      $zero, 0x37D8($v0)
    move    $v0, $t3
    sh      $v0, -0x6D14($t1)
.Lloop:
    lh      $v1, -0x6D14($t1)
    sh      $t3, 0x32($a3)
    addiu   $v1, $v1, -2
    subu    $v1, $t0, $v1
    srl     $v0, $v1, 31
    addu    $v0, $v1, $v0
    sra     $a0, $v0, 1
    sll     $v0, $a0, 1
    subu    $a0, $v1, $v0
    sll     $v0, $a0, 1
    addu    $v0, $v0, $a0
    sll     $v0, $v0, 2
    addu    $v0, $v0, $t2
    lw      $v1, 0($v0)
    lw      $a0, 4($v0)
    lw      $a1, 8($v0)
    sw      $v1, 0($a3)
    sw      $a0, 4($a3)
    sw      $a1, 8($a3)
    addiu   $t0, $t0, 1
    slti    $v0, $t0, 2
    bnez    $v0, .Lloop
     addiu  $a3, $a3, 0x3C
    lui     $v1, 0x8005
.Lfinish:
    li      $v0, 0x1B3
    lw      $ra, 0x10($sp)
    sw      $v0, 0x4514($v1)
    jr      $ra
     addiu  $sp, $sp, 0x18
.end func_8002D404
