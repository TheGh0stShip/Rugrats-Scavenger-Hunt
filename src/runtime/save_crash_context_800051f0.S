.set noreorder
.set noat
.text

.globl func_800051F0
.ent func_800051F0
func_800051F0:
    sd      $zero, 0x000($k0)
    sd      $at,   0x008($k0)
    sd      $v0,   0x010($k0)
    sd      $v1,   0x018($k0)
    sd      $a0,   0x020($k0)
    sd      $a1,   0x028($k0)
    sd      $a2,   0x030($k0)
    sd      $a3,   0x038($k0)
    sd      $t0,   0x040($k0)
    sd      $t1,   0x048($k0)
    sd      $t2,   0x050($k0)
    sd      $t3,   0x058($k0)
    sd      $t4,   0x060($k0)
    sd      $t5,   0x068($k0)
    sd      $t6,   0x070($k0)
    sd      $t7,   0x078($k0)
    sd      $s0,   0x080($k0)
    sd      $s1,   0x088($k0)
    sd      $s2,   0x090($k0)
    sd      $s3,   0x098($k0)
    sd      $s4,   0x0A0($k0)
    sd      $s5,   0x0A8($k0)
    sd      $s6,   0x0B0($k0)
    sd      $s7,   0x0B8($k0)
    sd      $t8,   0x0C0($k0)
    sd      $t9,   0x0C8($k0)
    sd      $k1,   0x0D8($k0)
    sd      $gp,   0x0E0($k0)
    sd      $sp,   0x0E8($k0)
    sd      $fp,   0x0F0($k0)
    mfc0    $t0, $30
    sd      $t0, 0x0D0($k0)
    mflo    $t0
    sd      $t0, 0x100($k0)
    mfhi    $t0
    sd      $t0, 0x108($k0)

    mfc0    $t0, $0
    nop
    sd      $t0, 0x118($k0)
    mfc0    $t0, $1
    nop
    sd      $t0, 0x120($k0)
    mfc0    $t0, $2
    nop
    sd      $t0, 0x128($k0)
    mfc0    $t0, $3
    nop
    sd      $t0, 0x130($k0)
    mfc0    $t0, $4
    nop
    sd      $t0, 0x138($k0)
    mfc0    $t0, $5
    nop
    sd      $t0, 0x140($k0)
    mfc0    $t0, $6
    nop
    sd      $t0, 0x148($k0)
    mfc0    $t0, $7
    nop
    sd      $t0, 0x150($k0)
    mfc0    $t0, $8
    nop
    sd      $t0, 0x158($k0)
    mfc0    $t0, $9
    nop
    sd      $t0, 0x160($k0)
    mfc0    $t0, $10
    nop
    sd      $t0, 0x168($k0)
    mfc0    $t0, $11
    nop
    sd      $t0, 0x170($k0)
    mfc0    $t0, $12
    nop
    sd      $t0, 0x178($k0)
    mfc0    $t0, $13
    nop
    sd      $t0, 0x180($k0)
    mfc0    $t0, $14
    nop
    sd      $t0, 0x188($k0)
    mfc0    $t0, $15
    nop
    sd      $t0, 0x190($k0)
    mfc0    $t0, $16
    nop
    sd      $t0, 0x198($k0)
    mfc0    $t0, $17
    nop
    sd      $t0, 0x1A0($k0)
    mfc0    $t0, $18
    nop
    sd      $t0, 0x1A8($k0)
    mfc0    $t0, $19
    nop
    sd      $t0, 0x1B0($k0)
    mfc0    $t0, $20
    nop
    sd      $t0, 0x1B8($k0)
    mfc0    $t0, $21
    nop
    sd      $t0, 0x1C0($k0)
    mfc0    $t0, $22
    nop
    sd      $t0, 0x1C8($k0)
    mfc0    $t0, $23
    nop
    sd      $t0, 0x1D0($k0)
    mfc0    $t0, $24
    nop
    sd      $t0, 0x1D8($k0)
    mfc0    $t0, $25
    nop
    sd      $t0, 0x1E0($k0)
    mfc0    $t0, $26
    nop
    sd      $t0, 0x1E8($k0)
    mfc0    $t0, $27
    nop
    sd      $t0, 0x1F0($k0)
    mfc0    $t0, $28
    nop
    sd      $t0, 0x1F8($k0)
    mfc0    $t0, $29
    nop
    sd      $t0, 0x200($k0)
    mfc0    $t0, $30
    nop
    sd      $t0, 0x208($k0)
    mfc0    $t0, $31
    nop
    sd      $t0, 0x210($k0)

    ld      $t0, 0x178($k0)
    lui     $t1, 0x2000
    and     $t1, $t1, $t0
    beqz    $t1, odd_fp_done
     nop
    cfc1    $t0, $0
    nop
    sw      $t0, 0x318($k0)
    cfc1    $t0, $31
    nop
    sw      $t0, 0x31C($k0)
    sdc1    $f0,  0x218($k0)
    sdc1    $f2,  0x228($k0)
    sdc1    $f4,  0x238($k0)
    sdc1    $f6,  0x248($k0)
    sdc1    $f8,  0x258($k0)
    sdc1    $f10, 0x268($k0)
    sdc1    $f12, 0x278($k0)
    sdc1    $f14, 0x288($k0)
    sdc1    $f16, 0x298($k0)
    sdc1    $f18, 0x2A8($k0)
    sdc1    $f20, 0x2B8($k0)
    sdc1    $f22, 0x2C8($k0)
    sdc1    $f24, 0x2D8($k0)
    sdc1    $f26, 0x2E8($k0)
    sdc1    $f28, 0x2F8($k0)
    sdc1    $f30, 0x308($k0)
even_fp_done:
    ld      $t0, 0x178($k0)
    lui     $t1, 0x0400
    and     $t1, $t1, $t0
    beqz    $t1, odd_fp_done
     nop
    sdc1    $f1,  0x220($k0)
    sdc1    $f3,  0x230($k0)
    sdc1    $f5,  0x240($k0)
    sdc1    $f7,  0x250($k0)
    sdc1    $f9,  0x260($k0)
    sdc1    $f11, 0x270($k0)
    sdc1    $f13, 0x280($k0)
    sdc1    $f15, 0x290($k0)
    sdc1    $f17, 0x2A0($k0)
    sdc1    $f19, 0x2B0($k0)
    sdc1    $f21, 0x2C0($k0)
    sdc1    $f23, 0x2D0($k0)
    sdc1    $f25, 0x2E0($k0)
    sdc1    $f27, 0x2F0($k0)
    sdc1    $f29, 0x300($k0)
    sdc1    $f31, 0x310($k0)
odd_fp_done:
    ld      $t0, 0x178($k0)
    nop
    andi    $t0, $t0, 4
    bnez    $t0, use_error_epc
     nop
    ld      $t0, 0x188($k0)
    j       save_restart_pc
     nop
use_error_epc:
    ld      $t0, 0x208($k0)
    nop
save_restart_pc:
    sd      $t0, 0x110($k0)
    jr      $ra
     nop
.end func_800051F0
