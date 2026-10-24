.set noat
.set noreorder
.text
.globl func_8002B434
.ent func_8002B434
func_8002B434:
    addiu   $sp, $sp, -0x38
    sw      $s0, 0x30($sp)
    sw      $ra, 0x34($sp)
    jal     func_8002A5EC
     move   $s0, $a0
    move    $t0, $zero
    li      $t2, 6
    lui     $v0, 0x8005
    addiu   $t1, $v0, 0x44E0
    move    $a3, $t1
    lui     $at, 0x8000
    lwc1    $f4, 0x2354($at)
.Lloop:
    sh      $t2, 0x30($a3)
    lw      $v0, 0($a3)
    lw      $v1, 4($a3)
    lw      $a0, 8($a3)
    sw      $v0, 0x10($sp)
    sw      $v1, 0x14($sp)
    sw      $a0, 0x18($sp)
    bne     $s0, $t2, .Lforward
     addiu  $v1, $t0, 1
    addiu   $v1, $t0, -1
    bltzl   $v1, .Lindex
     li     $v1, 1
    j       .Lcalculate
     sll    $v0, $v1, 4
.Lforward:
    slti    $v0, $v1, 2
    beqzl   $v0, .Lindex
     move   $v1, $zero
.Lindex:
    sll     $v0, $v1, 4
.Lcalculate:
    subu    $v0, $v0, $v1
    sll     $v0, $v0, 2
    addu    $v0, $v0, $t1
    lw      $v1, 0($v0)
    lw      $a0, 4($v0)
    lw      $a1, 8($v0)
    sw      $v1, 0x20($sp)
    sw      $a0, 0x24($sp)
    sw      $a1, 0x28($sp)
    lwc1    $f0, 0x20($sp)
    lwc1    $f2, 0x10($sp)
    sub.s   $f0, $f0, $f2
    div.s   $f0, $f0, $f4
    swc1    $f0, 0x24($a3)
    lwc1    $f0, 0x24($sp)
    lwc1    $f2, 0x14($sp)
    sub.s   $f0, $f0, $f2
    div.s   $f0, $f0, $f4
    swc1    $f0, 0x28($a3)
    lwc1    $f0, 0x28($sp)
    lwc1    $f2, 0x18($sp)
    sub.s   $f0, $f0, $f2
    div.s   $f0, $f0, $f4
    swc1    $f0, 0x2C($a3)
    addiu   $t0, $t0, 1
    slti    $v0, $t0, 2
    bnez    $v0, .Lloop
     addiu  $a3, $a3, 0x3C
    lw      $ra, 0x34($sp)
    lw      $s0, 0x30($sp)
    addiu   $sp, $sp, 0x38
    jr      $ra
     nop
.end func_8002B434
