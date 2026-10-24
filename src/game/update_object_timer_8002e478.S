.set noat
.set noreorder
.text
.globl func_8002E478
.ent func_8002E478
func_8002E478:
    lh      $v0, 0x194($a0)
    lhu     $v1, 0x194($a0)
    blez    $v0, .Lexpired
     addiu  $v0, $v1, -1
    j       .Lreturn
     sh     $v0, 0x194($a0)
.Lexpired:
    lui     $v0, %hi(D_800B92F4)
    lw      $v1, %lo(D_800B92F4)($v0)
    li      $v0, 0x28
    beq     $a1, $v1, .Lflag10
     sh     $v0, 0x194($a0)
    li      $v0, 1
    beq     $v1, $v0, .Lflag2
     nop
    slti    $v0, $v1, 2
    beqz    $v0, .Lhigh_modes
     li     $a2, 2
    beqz    $v1, .Lflag10
     nop
    j       .Lreturn
     nop
.Lhigh_modes:
    beq     $v1, $a2, .Lflag8
     nop
    li      $v0, 3
    beq     $v1, $v0, .Lmode3
     nop
    j       .Lreturn
     nop
.Lflag2:
    lw      $v0, 0x18C($a0)
    j       .Lstore
     ori    $v0, $v0, 2
.Lflag8:
    lw      $v0, 0x18C($a0)
    j       .Lstore
     ori    $v0, $v0, 8
.Lmode3:
    beqz    $a1, .Lflag8_mode3
     nop
    beq     $a1, $a2, .Lflag2_mode3
     nop
    j       .Lreturn
     nop
.Lflag8_mode3:
    lw      $v0, 0x18C($a0)
    j       .Lstore
     ori    $v0, $v0, 8
.Lflag2_mode3:
    lw      $v0, 0x18C($a0)
    j       .Lstore
     ori    $v0, $v0, 2
.Lflag10:
    lw      $v0, 0x18C($a0)
    ori     $v0, $v0, 0x10
.Lstore:
    sw      $v0, 0x18C($a0)
.Lreturn:
    jr      $ra
     nop
.end func_8002E478
