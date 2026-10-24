.set noat
.set noreorder
.text
.globl func_80009194
.ent func_80009194
func_80009194:
    addiu   $sp, $sp, -0x38
    sw      $s7, 0x2C($sp)
    move    $s7, $a1
    sw      $s3, 0x1C($sp)
    move    $s3, $zero
    sw      $s0, 0x10($sp)
    move    $s0, $s3
    lui     $v1, 0x8006
    sll     $a0, $a0, 16
    sra     $a0, $a0, 16
    sll     $v0, $a0, 5
    addu    $v0, $v0, $a0
    lw      $v1, -0x7E10($v1)
    sll     $a1, $v0, 4
    sw      $ra, 0x30($sp)
    sw      $s6, 0x28($sp)
    sw      $s5, 0x24($sp)
    sw      $s4, 0x20($sp)
    sw      $s2, 0x18($sp)
    sw      $s1, 0x14($sp)
    addu    $v1, $v1, $a1
    lh      $v0, 0x201E($v1)
    blez    $v0, .Ldone
     move   $s1, $s0
    li      $s6, -1
    move    $s5, $a1
    move    $s4, $a0
    lui     $v1, 0x8005
    addiu   $v1, $v1, 0x7F70
    sll     $v0, $s4, 6
    addu    $v0, $v0, $v1
    addiu   $s2, $v0, 4
.Lloop:
    move    $a0, $s4
    jal     func_8000E02C
     move   $a1, $s0
    sll     $v0, $v0, 16
    sra     $v0, $v0, 16
    beq     $v0, $s6, .Lcontinue
     lui    $v0, 0x8006
    lh      $v0, 0($s2)
    lhu     $v1, 0($s2)
    beqz    $v0, .Lempty
     addiu  $v0, $v1, -0x17C
    sltiu   $v0, $v0, 4
    beqz    $v0, .Lcontinue_setup
     move   $s1, $s0
    j       .Ldone
     li     $s3, 2
.Lcontinue_setup:
    j       .Lcontinue
     lui    $v0, 0x8006
.Lempty:
    bgtz    $s3, .Lcontinue
     lui    $v0, 0x8006
    move    $s1, $s0
    li      $s3, 1
.Lcontinue:
    lw      $v0, -0x7E10($v0)
    addu    $v0, $v0, $s5
    lh      $v0, 0x201E($v0)
    addiu   $s0, $s0, 1
    slt     $v0, $s0, $v0
    bnez    $v0, .Lloop
     addiu  $s2, $s2, 2
.Ldone:
    bnezl   $s7, .Lresult
     sw     $s3, 0($s7)
.Lresult:
    move    $v0, $s1
    lw      $ra, 0x30($sp)
    lw      $s7, 0x2C($sp)
    lw      $s6, 0x28($sp)
    lw      $s5, 0x24($sp)
    lw      $s4, 0x20($sp)
    lw      $s3, 0x1C($sp)
    lw      $s2, 0x18($sp)
    lw      $s1, 0x14($sp)
    lw      $s0, 0x10($sp)
    addiu   $sp, $sp, 0x38
    jr      $ra
     nop
.end func_80009194
