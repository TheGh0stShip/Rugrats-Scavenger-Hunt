/*
 * Host-debugger stub exception handler (func_80004CBC).
 *
 * Entered from the general exception vector with $k0 free.  The stub keeps a
 * private context block (DBG_CTX) holding the interrupted register file, and
 * talks to the host through a mailbox register in cartridge space (CART_MBOX)
 * guarded by the PI status busy bits.
 *
 *  - Exceptions whose ExcCode is enabled in DBG_EXC_MASK, and pending cartridge
 *    interrupts (IP3) carrying a host request, enter the stub.
 *  - BREAK instructions with a code of 0x400..0x408 are stub service calls,
 *    dispatched through break_service_table.
 *  - Anything else is passed on to the normal OS handler (os_exception_entry).
 */

.set noreorder
.set noat
.text

/* Stub context block and its fields. */
/* The assembler cannot split absolute constants, so give both halves. */
#define DBG_CTX_HI       0x8005         /* DBG_CTX = 0x80057290 */
#define DBG_CTX_LO       0x7290
#define DBG_STACK_HI     0x8005         /* stub stack top = 0x80057C20 */
#define DBG_STACK_LO     0x7C20
#define CTX_GPR(n)       ((n) * 8)      /* $1..$31 saved as doublewords */
#define CTX_LO           0x100
#define CTX_HI           0x108
#define CTX_EPC          0x110
#define CTX_STATUS       0x178
#define CTX_FPR(n)       (0x218 + (n) * 8)
#define CTX_FCR0         0x318
#define CTX_FCR31        0x31C
#define CTX_EXC_CODE     0x320          /* byte: reason reported to host */
#define CTX_EXC_FLAG     0x322          /* byte */
#define CTX_SAVE_AT      0x328
#define CTX_SAVE_V0      0x330
#define DBG_EXC_MASK     0x338          /* bit n set: trap ExcCode n */
#define DBG_ACTIVE       0x33C          /* nonzero while the stub owns the CPU */

/* Hardware. */
#define PI_STATUS_HI     0xA460         /* PI_STATUS_REG = 0xA4600010 */
#define PI_STATUS_LO     0x0010
#define PI_STATUS_BUSY   3
#define CART_MBOX_HI     0xB200
#define CART_MBOX_CMD    -0x28          /* 0xB1FFFFD8 */
#define CART_MBOX_REQ    -0x10          /* 0xB1FFFFF0 */
#define HANDSHAKE_HI     0x0303         /* handshake word 0x03030303 */
#define HANDSHAKE_LO     0x0303
#define HOST_HELLO       0x0202
#define CAUSE_IP3        0x0800
#define CAUSE_SW1        0x0100
#define EXC_BREAK        9
#define BREAK_SVC_BASE   0x400
#define BREAK_SVC_COUNT  9

/* Code outside this unit, addressed relative to it. */
.set os_exception_entry, func_80004CBC - 0x158     /* 0x80004B64 */
.set vector_stub_a,      func_80004CBC - 0x158     /* 0x80004B64: copied to 0x80000180 */
.set vector_stub_b,      func_80004CBC - 0x120     /* 0x80004B9C: copied to 0x80000000 */
.set host_hook,          func_80004CBC - 0x930     /* 0x8000438C */
.set stub_main,          func_80004CBC + 0x534     /* 0x800051F0 */

/* Spin until the PI is idle (clobbers $at). */
#define PI_WAIT(lbl)                    \
lbl:                                    \
    lui     $at, PI_STATUS_HI;        \
    ori     $at, $at, PI_STATUS_LO;   \
    lw      $at, 0($at);                \
    nop;                                \
    andi    $at, $at, PI_STATUS_BUSY;   \
    bnez    $at, lbl;                   \
     nop

.globl func_80004CBC
.ent func_80004CBC
func_80004CBC:
    lui     $k0, DBG_CTX_HI
    addiu   $k0, $k0, DBG_CTX_LO
    sd      $at, CTX_SAVE_AT($k0)
    sd      $v0, CTX_SAVE_V0($k0)
    /* Is this ExcCode trapped by the stub? */
    mfc0    $at, $13
    lw      $v0, DBG_EXC_MASK($k0)
    srl     $at, $at, 2
    andi    $at, $at, 0x1F
    beqz    $at, check_interrupt         /* ExcCode 0: interrupt */
     srlv   $v0, $v0, $at
    andi    $v0, $v0, 1
    bnez    $v0, check_break
     nop
pass_to_os:
    ld      $at, CTX_SAVE_AT($k0)
    ld      $v0, CTX_SAVE_V0($k0)
    bgez    $zero, os_exception_entry
     nop

check_interrupt:
    mfc0    $at, $13
    nop
    andi    $at, $at, CAUSE_IP3          /* cartridge interrupt pending? */
    beqz    $at, pass_to_os
     nop
    PI_WAIT(wait_pi_hello)
    lui     $at, CART_MBOX_HI
    lw      $at, CART_MBOX_CMD($at)
    nop
    andi    $at, $at, 0xFFFF
    addi    $at, $at, -HOST_HELLO
    bnez    $at, check_request           /* not a connect request */
     nop
handshake:
    PI_WAIT(wait_pi_send)
    lui     $at, HANDSHAKE_HI
    ori     $at, $at, HANDSHAKE_LO
    lui     $v0, CART_MBOX_HI
    sw      $at, CART_MBOX_CMD($v0)
    PI_WAIT(wait_pi_echo)
    lui     $v0, CART_MBOX_HI
    lw      $at, CART_MBOX_CMD($v0)
    lui     $v0, HANDSHAKE_HI
    ori     $v0, $v0, HANDSHAKE_LO
    sub     $at, $at, $v0
    bnez    $at, handshake_done          /* host acknowledged */
     nop
    lui     $at, 1                       /* otherwise wait and re-read */
    ori     $at, $at, 1
1:  bnez    $at, 1b
     addi   $at, $at, -1
    beqz    $at, wait_pi_echo            /* then poll for the echo again */
     nop
handshake_done:
    PI_WAIT(wait_pi_clear)
    lui     $v0, CART_MBOX_HI
    sw      $zero, CART_MBOX_CMD($v0)
    /* Raise software interrupt 1 and let the OS see the interrupt too. */
    mfc0    $at, $13
    nop
    ori     $at, $at, CAUSE_SW1
    j       pass_to_os
     mtc0   $at, $13

check_request:
    PI_WAIT(wait_pi_req)
    lui     $at, CART_MBOX_HI
    lw      $at, CART_MBOX_REQ($at)
    nop
    andi    $at, $at, 0xFFFF
    beqz    $at, pass_to_os              /* no host request pending */
     nop
    bgez    $zero, enter_stub_active            /* host break-in */
     nop

check_break:
    addiu   $v0, $zero, EXC_BREAK
    bne     $at, $v0, enter_stub
     nop
    mfc0    $v0, $14                     /* decode the BREAK code */
    nop
    lw      $v0, 0($v0)
    nop
    srl     $v0, $v0, 6
    addiu   $at, $v0, -BREAK_SVC_BASE
    bgez    $at, break_service
     nop
enter_stub_break:
    addiu   $at, $zero, EXC_BREAK
enter_stub:
    sb      $at, CTX_EXC_CODE($k0)
    sb      $zero, CTX_EXC_FLAG($k0)
enter_stub_active:                       /* host break-in keeps CTX_EXC_CODE */
    addiu   $at, $zero, 1
    sw      $at, DBG_ACTIVE($k0)
    ld      $at, CTX_SAVE_AT($k0)
    ld      $v0, CTX_SAVE_V0($k0)
    sd      $ra, CTX_GPR(31)($k0)
    jal     stub_main                    /* save full context */
     nop
    lui     $sp, DBG_STACK_HI
    addiu   $sp, $sp, DBG_STACK_LO
    jal     func_80004458                /* talk to the host */
     nop
    jal     func_80004C6C
     nop
    jal     func_80004C88
     nop

/* Restore the interrupted context and return. */
restore_context:
    lui     $k0, DBG_CTX_HI
    addiu   $k0, $k0, DBG_CTX_LO
    addiu   $at, $zero, 1
    sb      $at, CTX_EXC_CODE($k0)
    ld      $t0, CTX_HI($k0)
    nop
    mthi    $t0
    ld      $t0, CTX_LO($k0)
    nop
    mtlo    $t0
    ld      $t0, CTX_STATUS($k0)
    nop
    mtc0    $t0, $12
    ld      $t0, CTX_EPC($k0)
    nop
    mtc0    $t0, $14
    ld      $t0, CTX_GPR(26)($k0)
    nop
    mtc0    $t0, $30                     /* resume address via ErrorEPC */
    ld      $t0, CTX_STATUS($k0)
    lui     $t1, 0x2000                  /* SR_CU1 */
    and     $t1, $t1, $t0
    beqz    $t1, restore_gprs
     nop
    lw      $t0, CTX_FCR0($k0)
    nop
    ctc1    $t0, $0
    lw      $t0, CTX_FCR31($k0)
    nop
    ctc1    $t0, $31
    ldc1    $f0, CTX_FPR(0)($k0)
    ldc1    $f2, CTX_FPR(2)($k0)
    ldc1    $f4, CTX_FPR(4)($k0)
    ldc1    $f6, CTX_FPR(6)($k0)
    ldc1    $f8, CTX_FPR(8)($k0)
    ldc1    $f10, CTX_FPR(10)($k0)
    ldc1    $f12, CTX_FPR(12)($k0)
    ldc1    $f14, CTX_FPR(14)($k0)
    ldc1    $f16, CTX_FPR(16)($k0)
    ldc1    $f18, CTX_FPR(18)($k0)
    ldc1    $f20, CTX_FPR(20)($k0)
    ldc1    $f22, CTX_FPR(22)($k0)
    ldc1    $f24, CTX_FPR(24)($k0)
    ldc1    $f26, CTX_FPR(26)($k0)
    ldc1    $f28, CTX_FPR(28)($k0)
    ldc1    $f30, CTX_FPR(30)($k0)
    ld      $t0, CTX_STATUS($k0)
    lui     $t1, 0x0400                  /* SR_FR: odd FPRs too */
    and     $t1, $t1, $t0
    beqz    $t1, restore_gprs
     nop
    ldc1    $f1, CTX_FPR(1)($k0)
    ldc1    $f3, CTX_FPR(3)($k0)
    ldc1    $f5, CTX_FPR(5)($k0)
    ldc1    $f7, CTX_FPR(7)($k0)
    ldc1    $f9, CTX_FPR(9)($k0)
    ldc1    $f11, CTX_FPR(11)($k0)
    ldc1    $f13, CTX_FPR(13)($k0)
    ldc1    $f15, CTX_FPR(15)($k0)
    ldc1    $f17, CTX_FPR(17)($k0)
    ldc1    $f19, CTX_FPR(19)($k0)
    ldc1    $f21, CTX_FPR(21)($k0)
    ldc1    $f23, CTX_FPR(23)($k0)
    ldc1    $f25, CTX_FPR(25)($k0)
    ldc1    $f27, CTX_FPR(27)($k0)
    ldc1    $f29, CTX_FPR(29)($k0)
    ldc1    $f31, CTX_FPR(31)($k0)
restore_gprs:
    ld      $1, CTX_GPR(1)($k0)
    ld      $2, CTX_GPR(2)($k0)
    ld      $3, CTX_GPR(3)($k0)
    ld      $4, CTX_GPR(4)($k0)
    ld      $5, CTX_GPR(5)($k0)
    ld      $6, CTX_GPR(6)($k0)
    ld      $7, CTX_GPR(7)($k0)
    ld      $8, CTX_GPR(8)($k0)
    ld      $9, CTX_GPR(9)($k0)
    ld      $10, CTX_GPR(10)($k0)
    ld      $11, CTX_GPR(11)($k0)
    ld      $12, CTX_GPR(12)($k0)
    ld      $13, CTX_GPR(13)($k0)
    ld      $14, CTX_GPR(14)($k0)
    ld      $15, CTX_GPR(15)($k0)
    ld      $16, CTX_GPR(16)($k0)
    ld      $17, CTX_GPR(17)($k0)
    ld      $18, CTX_GPR(18)($k0)
    ld      $19, CTX_GPR(19)($k0)
    ld      $20, CTX_GPR(20)($k0)
    ld      $21, CTX_GPR(21)($k0)
    ld      $22, CTX_GPR(22)($k0)
    ld      $23, CTX_GPR(23)($k0)
    ld      $24, CTX_GPR(24)($k0)
    ld      $25, CTX_GPR(25)($k0)
    ld      $27, CTX_GPR(27)($k0)
    ld      $28, CTX_GPR(28)($k0)
    ld      $29, CTX_GPR(29)($k0)
    ld      $30, CTX_GPR(30)($k0)
    ld      $31, CTX_GPR(31)($k0)
    sw      $zero, DBG_ACTIVE($k0)
    mfc0    $k0, $30
    eret

/* BREAK service calls 0x400..0x408. */
break_service:
    addiu   $at, $v0, -(BREAK_SVC_BASE + BREAK_SVC_COUNT)
    bgez    $at, enter_stub_break        /* out of range: plain break */
     nop
    lui     $at, %hi(break_service_table)
    addiu   $at, $at, %lo(break_service_table)
    addiu   $v0, $v0, -BREAK_SVC_BASE
    sll     $v0, $v0, 2
    addu    $v0, $v0, $at
    lw      $v0, 0($v0)
    nop
    jr      $v0
     nop

break_service_table:
    .word   svc_host_poll                /* 0x400 */
    .word   svc_fault_a                  /* 0x401 */
    .word   svc_fault_b                  /* 0x402 */
    .word   enter_stub_break             /* 0x403 */
    .word   svc_break                    /* 0x404 */
    .word   svc_install_vectors          /* 0x405 */
    .word   svc_set_exc_mask             /* 0x406 */
    .word   svc_skip                     /* 0x407 */
    .word   svc_host_hook                /* 0x408 */

/* 0x400: if the host has a request pending, break into the stub. */
svc_host_poll:
    lw      $at, DBG_ACTIVE($k0)
    nop
    bnez    $at, skip_restore_v0         /* already inside the stub */
     nop
    addiu   $at, $zero, 100
1:  bnez    $at, 1b
     addi   $at, $at, -1
    lui     $at, PI_STATUS_HI
    ori     $at, $at, PI_STATUS_LO
2:  lw      $v0, 0($at)
    nop
    andi    $v0, $v0, PI_STATUS_BUSY
    bnez    $v0, 2b
     nop
    lui     $at, CART_MBOX_HI
    lw      $at, CART_MBOX_REQ($at)
    nop
    beqz    $at, skip_restore_v0
     nop
    bgez    $zero, enter_stub_active
     nop

svc_fault_a:
    addiu   $at, $zero, -1
    j       enter_stub
     nop
svc_fault_b:
    addiu   $at, $zero, -1
    j       enter_stub
     nop
svc_break:
    j       enter_stub_break
     nop

/* 0x405: copy the stub's vector code into the exception vectors. */
svc_install_vectors:
    sd      $v1, CTX_GPR(3)($k0)
    sd      $a0, CTX_GPR(4)($k0)
    sd      $ra, CTX_GPR(31)($k0)
    bal     install_vectors
     nop
    ld      $ra, CTX_GPR(31)($k0)
    ld      $a0, CTX_GPR(4)($k0)
    bgez    $zero, skip_restore_v0
     ld     $v1, CTX_GPR(3)($k0)

install_vectors:
    lui     $v1, 0x8000
    ori     $v1, $v1, 0x0180             /* general exception vector */
    lui     $v0, %hi(vector_stub_a)
    addiu   $v0, $v0, %lo(vector_stub_a)
    addi    $a0, $v0, 0x14
1:  lw      $at, 0($v0)
    addi    $v0, $v0, 4
    sw      $at, 0($v1)
    addi    $v1, $v1, 4
    bne     $v0, $a0, 1b
     nop
    lui     $v1, 0x8000                  /* TLB refill vector */
    lui     $v0, %hi(vector_stub_b)
    addiu   $v0, $v0, %lo(vector_stub_b)
    addi    $a0, $v0, 0x14
2:  lw      $at, 0($v0)
    addi    $v0, $v0, 4
    sw      $at, 0($v1)
    addi    $v1, $v1, 4
    bne     $v0, $a0, 2b
     nop
    jal     func_80004C6C
     nop
    jal     func_80004C88
     nop
    jr      $ra
     nop

/* 0x406: $a0 = new trapped-ExcCode mask. */
svc_set_exc_mask:
    sw      $a0, DBG_EXC_MASK($k0)
    j       skip_restore_v0
     nop

/* 0x407: step over the BREAK and enter the stub with code 0. */
svc_skip:
    mfc0    $at, $14
    nop
    addiu   $at, $at, 4
    mtc0    $at, $14
    addiu   $at, $zero, 0
    j       enter_stub
     nop

/* 0x408: return the host hook address in $v0. */
svc_host_hook:
    lui     $v0, %hi(host_hook)
    addiu   $v0, $v0, %lo(host_hook)
    j       skip_break
     nop

/* Step over the BREAK and resume without entering the stub. */
skip_restore_v0:
    ld      $v0, CTX_SAVE_V0($k0)
skip_break:
    mfc0    $at, $14
    nop
    addiu   $at, $at, 4
    mtc0    $at, $14
    ld      $at, CTX_SAVE_AT($k0)
    mfc0    $k0, $30
    eret
.end func_80004CBC
