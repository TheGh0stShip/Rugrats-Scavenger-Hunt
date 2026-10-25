// n_aspMain: N_audio (libmus / n_audio) RSP audio microcode, IMEM text.
// Rugrats retail ROM 0x500-0x1160 (0xC60 bytes), loaded by the RSP task at
// IMEM 0x04001080.
//
// Reverse-engineered source: every word is an RSP instruction written symbolically, plus
// 12 bytes of explicit object padding. Assembles with armips 0.11 (.rsp) byte-for-byte to the
// retail image. Region/handler names are inferences and are marked as such.
// The companion DMEM data (dispatch table, constants) is n_aspMainDataStart in
// src/libmus/audio_runtime_data.c.

.rsp

// Build: armips -strequ CODE_FILE <output.bin> n_aspMain.s  (same convention as sm64 rsp/audio.s)

.create CODE_FILE, 0x04001080

n_aspMainTextStart:

    mfc0    $5, DPC_STATUS
    lw      $28, 0x30($1)
    lw      $27, 0x34($1)
    andi    $4, $5, 0x1
    beqz    $4, boot_load_first_chunk
    andi    $4, $5, 0x100
    beqz    $4, boot_load_first_chunk
    mfc0    $4, DPC_STATUS
boot_wait_rdp_dma:
    andi    $4, $4, 0x100
    bgtz    $4, boot_wait_rdp_dma
    mfc0    $4, DPC_STATUS
boot_load_first_chunk:
    addi    $24, $zero, 0xFA0
    jal     load_command_chunk
    add     $2, $zero, $28
    mfc0    $2, SP_DMA_BUSY

// Wait for the command-list DMA, release the semaphore.
wait_chunk_dma:
    bnez    $2, wait_chunk_dma
    mfc0    $2, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE

// Command dispatcher: $26/$25 = command words 0/1 at DMEM[$29]. Opcode = w0 >> 24; the handler
// address is the halfword at DMEM[0x000 + opcode*2] (the commandHandlers table). $27 = bytes left in
// the list, $28 = DRAM pointer of the next chunk, $30 = bytes left in the current DMEM chunk.
dispatch_loop:
    lw      $26, 0x0($29)
    lw      $25, 0x4($29)
    addi    $28, $28, 0x8
    srl     $1, $26, 23
    andi    $1, $1, 0xFE
    lh      $1, 0x0($1)
    jr      $1
    addi    $27, $27, -0x8
    break   0

// Common handler tail (also opcode 0x00): advance to the next command; refill the 0x140-byte
// DMEM chunk at 0x2B0 when it is exhausted; finish when the list is empty.
cmd_SPNOOP_next_command:
    bgtz    $30, dispatch_loop
    addi    $29, $29, 0x8
    blez    $27, task_done
    ori     $1, $zero, 0x4000
    jal     load_command_chunk
    add     $2, $zero, $28
    j       wait_chunk_dma
    mfc0    $2, SP_DMA_BUSY

// List finished: SP_STATUS write 0x4000 (= set SIG2 / task-done signal in the RSP status write
// bit layout used by libultra), then break.
task_done:
    mtc0    $1, SP_STATUS    // write SP_STATUS ($1)
    break   0
    nop
halt_forever:
    b       halt_forever
    nop

// load_command_chunk($2 = DRAM address): DMA up to 0x140 bytes of the command list into DMEM 0x2B0.
load_command_chunk:
    addi    $5, $ra, 0x0
    addi    $3, $27, 0x0
    addi    $4, $3, -0x140
    blez    $4, .L_1138
    addi    $1, $zero, 0x2B0
    addi    $3, $zero, 0x140
.L_1138:
    addi    $30, $3, 0x0
    jal     dma_read
    addi    $3, $3, -0x1
    jr      $5
    addi    $29, $zero, 0x2B0

// dma_read($1 = DMEM, $2 = DRAM, $3 = length-1): take the semaphore, wait for a free DMA slot, start RD.
dma_read:
    mfc0    $4, SP_SEMAPHORE
.L_1150:
    bnez    $4, .L_1150
    mfc0    $4, SP_SEMAPHORE
    mfc0    $4, SP_DMA_FULL
.L_115C:
    bnez    $4, .L_115C
    mfc0    $4, SP_DMA_FULL
    mtc0    $1, SP_MEM_ADDR
    mtc0    $2, SP_DRAM_ADDR
    jr      $ra
    mtc0    $3, SP_RD_LEN

// dma_write($1 = DMEM, $2 = DRAM, $3 = length-1): as dma_read, but starts a WR (DMEM -> DRAM) DMA.
dma_write:
    mfc0    $4, SP_SEMAPHORE
.L_1178:
    bnez    $4, .L_1178
    mfc0    $4, SP_SEMAPHORE
    mfc0    $4, SP_DMA_FULL
.L_1184:
    bnez    $4, .L_1184
    mfc0    $4, SP_DMA_FULL
    mtc0    $1, SP_MEM_ADDR
    mtc0    $2, SP_DRAM_ADDR
    jr      $ra
    mtc0    $3, SP_WR_LEN

// Command handler for opcode(s) 0x02 CLEARBUFF (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_CLEARBUFF:
    andi    $2, $25, 0xFFFF
    vxor    $v1, $v1, $v1
    andi    $1, $26, 0xFFFF
    addi    $1, $1, 0x4F0
.L_11AC:
    sdv     $v1[0], 0x0($1)
    sdv     $v1[0], 0x8($1)
    addi    $2, $2, -0x10
    bgtz    $2, .L_11AC
    addi    $1, $1, 0x10
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x04 LOADBUFF (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_LOADBUFF:
    sll     $3, $26, 8
    srl     $3, $3, 20
    beqz    $3, cmd_SPNOOP_next_command
    addi    $30, $30, -0x8
    andi    $1, $26, 0xFFF
    addi    $1, $1, 0x4F0
    sll     $2, $25, 8
    srl     $2, $2, 8
    addi    $3, $3, -0x1
    jal     dma_read
    addi    $2, $2, 0x0
    mfc0    $1, SP_DMA_BUSY
.L_11F8:
    bnez    $1, .L_11F8
    mfc0    $1, SP_DMA_BUSY
    j       cmd_SPNOOP_next_command
    mtc0    $zero, SP_SEMAPHORE

// Command handler for opcode(s) 0x06 SAVEBUFF (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_SAVEBUFF:
    sll     $3, $26, 8
    srl     $3, $3, 20
    beqz    $3, cmd_SPNOOP_next_command
    addi    $30, $30, -0x8
    andi    $1, $26, 0xFFF
    addi    $1, $1, 0x4F0
    sll     $2, $25, 8
    srl     $2, $2, 8
    addi    $3, $3, -0x1
    jal     dma_write
    addi    $2, $2, 0x0
    mfc0    $1, SP_DMA_BUSY
.L_1238:
    bnez    $1, .L_1238
    mfc0    $1, SP_DMA_BUSY
    j       cmd_SPNOOP_next_command
    mtc0    $zero, SP_SEMAPHORE

// Command handler for opcode(s) 0x0B LOADADPCM (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_LOADADPCM:
    sll     $2, $25, 8
    srl     $2, $2, 8
    addi    $2, $2, 0x0
    addi    $1, $zero, 0x3F0
    andi    $3, $26, 0xFFFF
    jal     dma_read
    addi    $3, $3, -0x1
    mfc0    $1, SP_DMA_BUSY
.L_1268:
    bnez    $1, .L_1268
    mfc0    $1, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x09 SETVOL (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_SETVOL:
    srl     $3, $26, 16
    andi    $1, $3, 0x4
    beqz    $1, .L_12BC
    andi    $1, $3, 0x2
    beqz    $1, .L_12A8
    srl     $2, $25, 16
    sh      $26, 0x50($24)
    sh      $2, 0x4C($24)
    sh      $25, 0x4E($24)
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8
.L_12A8:
    sh      $26, 0x46($24)
    sh      $2, 0x48($24)

// Command handler for opcode(s) 0x0E OP14 (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_OP14:
    sh      $25, 0x4A($24)
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8
.L_12BC:
    srl     $2, $25, 16
    sh      $26, 0x40($24)
    sh      $2, 0x42($24)
    sh      $25, 0x44($24)
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x0D INTERLEAVE (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_INTERLEAVE:
    addi    $1, $zero, 0x170
    addi    $4, $zero, 0x4F0
    addi    $2, $zero, 0x9D0
    addi    $3, $zero, 0xB40
.L_12E4:
    lqv     $v1[0], 0x0($2)
    lqv     $v2[0], 0x0($3)
    addi    $1, $1, -0x10
    addi    $2, $2, 0x10
    addi    $3, $3, 0x10
    ssv     $v1[0], 0x0($4)
    ssv     $v2[0], 0x2($4)
    ssv     $v1[2], 0x4($4)
    ssv     $v2[2], 0x6($4)
    ssv     $v1[4], 0x8($4)
    ssv     $v2[4], 0xA($4)
    ssv     $v1[6], 0xC($4)
    ssv     $v2[6], 0xE($4)
    ssv     $v1[8], 0x10($4)
    ssv     $v2[8], 0x12($4)
    ssv     $v1[10], 0x14($4)
    ssv     $v2[10], 0x16($4)
    ssv     $v1[12], 0x18($4)
    ssv     $v2[12], 0x1A($4)
    ssv     $v1[14], 0x1C($4)
    ssv     $v2[14], 0x1E($4)
    bgtz    $1, .L_12E4
    addi    $4, $4, 0x20
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x0A DMEMMOVE (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_DMEMMOVE:
    andi    $1, $25, 0xFFFF
    andi    $2, $26, 0xFFFF
    addi    $2, $2, 0x4F0
    srl     $3, $25, 16
    addi    $3, $3, 0x4F0
.L_135C:
    ldv     $v1[0], 0x0($2)
    ldv     $v2[0], 0x8($2)
    addi    $1, $1, -0x10
    addi    $2, $2, 0x10
    sdv     $v1[0], 0x0($3)
    sdv     $v2[0], 0x8($3)
    bgtz    $1, .L_135C
    addi    $3, $3, 0x10
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x0F SETLOOP (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_SETLOOP:
    sll     $1, $25, 8
    srl     $1, $1, 8
    addi    $1, $1, 0x0
    sw      $1, 0xE($zero)
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x01 ADPCM (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_ADPCM:
    lqv     $v31[0], 0x50($zero)
    srl     $23, $25, 12
    vxor    $v25, $v25, $v25
    andi    $23, $23, 0xF
    vxor    $v24, $v24, $v24
    addi    $23, $23, 0x4F0
    vxor    $v13, $v13, $v13
    andi    $1, $25, 0xFFF
    vxor    $v14, $v14, $v14
    addi    $1, $1, 0x4F0
    vxor    $v15, $v15, $v15
    srl     $21, $25, 16
    vxor    $v16, $v16, $v16
    andi    $21, $21, 0xFFF
    vxor    $v17, $v17, $v17
    sll     $20, $26, 8
    vxor    $v18, $v18, $v18
    srl     $20, $20, 8
    vxor    $v19, $v19, $v19
    addi    $3, $zero, 0x1F
    srl     $13, $25, 28
    andi    $2, $13, 0x1
    bgtz    $2, .L_1460
    addi    $22, $23, 0x1
    andi    $2, $13, 0x2
    beqz    $2, .L_140C
    addi    $2, $20, 0x0
    lw      $2, 0xE($zero)
.L_140C:
    mfc0    $13, SP_SEMAPHORE
.L_1410:
    bnez    $13, .L_1410
    mfc0    $13, SP_SEMAPHORE
    mfc0    $13, SP_DMA_FULL
.L_141C:
    bnez    $13, .L_141C
    mfc0    $13, SP_DMA_FULL
    mtc0    $1, SP_MEM_ADDR
    mtc0    $2, SP_DRAM_ADDR
    mtc0    $3, SP_RD_LEN
    addi    $19, $zero, 0x20
    addi    $18, $zero, 0x3F0
    ldv     $v25[0], 0x0($19)
    ldv     $v24[8], 0x0($19)
    ldv     $v23[0], 0x8($19)
    ldv     $v23[8], 0x8($19)
    mfc0    $5, SP_DMA_BUSY
.L_144C:
    bnez    $5, .L_144C
    mfc0    $5, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    j       .L_1484
    lqv     $v27[0], 0x10($1)
.L_1460:
    addi    $19, $zero, 0x20
    vxor    $v27, $v27, $v27
    addi    $18, $zero, 0x3F0
    ldv     $v25[0], 0x0($19)
    ldv     $v24[8], 0x0($19)
    ldv     $v23[0], 0x8($19)
    ldv     $v23[8], 0x8($19)
    sqv     $v27[0], 0x0($1)
    sqv     $v27[0], 0x10($1)
.L_1484:
    beqz    $21, .L_1634
    addi    $1, $1, 0x20
    ldv     $v12[0], 0x0($22)
    lbu     $10, 0x0($23)
    addi    $13, $zero, 0xC
    addi    $12, $zero, 0x1
    andi    $14, $10, 0xF
    sll     $14, $14, 5
    vand    $v10, $v25, $v12[0]
    add     $16, $14, $18
    vand    $v9, $v24, $v12[1]
    srl     $17, $10, 4
    vand    $v8, $v25, $v12[2]
    sub     $17, $13, $17
    vand    $v7, $v24, $v12[3]
    addi    $13, $17, -0x1
    sll     $12, $12, 15
    srlv    $11, $12, $13
    mtc2    $11, $v22[0]
    lqv     $v21[0], 0x0($16)
    lqv     $v20[0], 0x10($16)
    addi    $16, $16, -0x2
    lrv     $v19[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v18[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v17[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v16[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v15[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v14[0], 0x20($16)
    addi    $16, $16, -0x2
    lrv     $v13[0], 0x20($16)
.L_1510:
    addi    $22, $22, 0x9
    vmudn   $v30, $v10, $v23
    addi    $23, $23, 0x9
    vmadn   $v30, $v9, $v23
    lbu     $10, 0x0($23)
    vmudn   $v29, $v8, $v23
    ldv     $v12[0], 0x0($22)
    vmadn   $v29, $v7, $v23
    addi    $13, $zero, 0xC
    blez    $17, .L_1544
    andi    $14, $10, 0xF
    vmudm   $v30, $v30, $v22[0]
    vmudm   $v29, $v29, $v22[0]
.L_1544:
    sll     $14, $14, 5
    vmudh   $v11, $v21, $v27[6]
    add     $16, $14, $18
    vmadh   $v11, $v20, $v27[7]
    vmadh   $v11, $v19, $v30[0]
    vmadh   $v11, $v18, $v30[1]
    srl     $17, $10, 4
    vmadh   $v11, $v17, $v30[2]
    vmadh   $v11, $v16, $v30[3]
    sub     $17, $13, $17
    vmadh   $v28, $v15, $v30[4]
    addi    $13, $17, -0x1
    vmadh   $v11, $v14, $v30[5]
    vmadh   $v11, $v13, $v30[6]
    vmadh   $v11, $v30, $v31[3]
    srlv    $11, $12, $13
    vsar    $v26, $v6, $v28[1]
    mtc2    $11, $v22[0]
    vsar    $v28, $v6, $v28[0]
    vand    $v10, $v25, $v12[0]
    vand    $v9, $v24, $v12[1]
    vand    $v8, $v25, $v12[2]
    vand    $v7, $v24, $v12[3]
    vmudn   $v11, $v26, $v31[1]
    vmadh   $v28, $v28, $v31[1]
    vmudh   $v11, $v19, $v29[0]
    addi    $15, $16, -0x2
    vmadh   $v11, $v18, $v29[1]
    lrv     $v19[0], 0x20($15)
    vmadh   $v11, $v17, $v29[2]
    addi    $15, $15, -0x2
    vmadh   $v11, $v16, $v29[3]
    lrv     $v18[0], 0x20($15)
    vmadh   $v11, $v15, $v29[4]
    addi    $15, $15, -0x2
    vmadh   $v11, $v14, $v29[5]
    lrv     $v17[0], 0x20($15)
    vmadh   $v11, $v13, $v29[6]
    addi    $15, $15, -0x2
    vmadh   $v11, $v29, $v31[3]
    lrv     $v16[0], 0x20($15)
    vmadh   $v11, $v21, $v28[6]
    addi    $15, $15, -0x2
    vmadh   $v11, $v20, $v28[7]
    lrv     $v15[0], 0x20($15)
    vsar    $v26, $v6, $v27[1]
    addi    $15, $15, -0x2
    vsar    $v27, $v6, $v27[0]
    lrv     $v14[0], 0x20($15)
    addi    $15, $15, -0x2
    lrv     $v13[0], 0x20($15)
    lqv     $v21[0], 0x0($16)
    vmudn   $v11, $v26, $v31[1]
    lqv     $v20[0], 0x10($16)
    vmadh   $v27, $v27, $v31[1]
    addi    $21, $21, -0x20
    sqv     $v28[0], 0x0($1)
    addi    $1, $1, 0x20
    bgtz    $21, .L_1510
    sqv     $v27[0], 0x7F0($1)
.L_1634:
    addi    $1, $1, -0x20
    jal     dma_write
    addi    $2, $20, 0x0
    addi    $30, $30, -0x8
    mfc0    $5, SP_DMA_BUSY
.L_1648:
    bnez    $5, .L_1648
    mfc0    $5, SP_DMA_BUSY
    j       cmd_SPNOOP_next_command
    mtc0    $zero, SP_SEMAPHORE
    srl     $19, $25, 24
    addi    $20, $zero, 0x3F0
    vxor    $v21, $v21, $v21
    beqz    $19, .L_1670
    addi    $23, $zero, 0x4F0
    addi    $23, $zero, 0x660
.L_1670:
    lqv     $v28[0], 0x10($20)
    vxor    $v22, $v22, $v22
    mtc2    $26, $v18[10]
    vxor    $v23, $v23, $v23
    sll     $26, $26, 2
    vxor    $v24, $v24, $v24
    mtc2    $26, $v20[0]
    vxor    $v25, $v25, $v25
    sll     $2, $25, 8
    vxor    $v26, $v26, $v26
    srl     $2, $2, 8
    vxor    $v27, $v27, $v27
    addi    $2, $2, 0x0
    addi    $3, $zero, 0x7
    addi    $19, $zero, 0x4
    mtc2    $19, $v18[0]
    addi    $22, $zero, 0x170
    vmudm   $v20, $v28, $v20[0]
    srl     $19, $26, 18
    andi    $19, $19, 0x1
    bgtz    $19, .L_1740
    sqv     $v20[0], 0x10($20)
    addi    $1, $24, 0x0
    mfc0    $19, SP_SEMAPHORE
.L_16D0:
    bnez    $19, .L_16D0
    mfc0    $19, SP_SEMAPHORE
    mfc0    $19, SP_DMA_FULL
.L_16DC:
    bnez    $19, .L_16DC
    mfc0    $19, SP_DMA_FULL
    mtc0    $1, SP_MEM_ADDR
    mtc0    $2, SP_DRAM_ADDR
    mtc0    $3, SP_RD_LEN
    addi    $20, $20, -0x2
    lrv     $v27[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v26[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v25[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v24[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v23[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v22[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v21[0], 0x20($20)
    mfc0    $5, SP_DMA_BUSY
.L_172C:
    bnez    $5, .L_172C
    mfc0    $5, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    j       .L_177C
    ldv     $v30[8], 0x0($1)
.L_1740:
    addi    $20, $20, -0x2
    vxor    $v30, $v30, $v30
    lrv     $v27[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v26[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v25[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v24[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v23[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v22[0], 0x20($20)
    addi    $20, $20, -0x2
    lrv     $v21[0], 0x20($20)
.L_177C:
    lqv     $v31[0], 0x0($23)
.L_1780:
    vmudh   $v20, $v28, $v30[7]
    vmadh   $v20, $v27, $v31[0]
    addi    $22, $22, -0x10
    vmadh   $v20, $v26, $v31[1]
    vmadh   $v20, $v25, $v31[2]
    sqv     $v30[0], 0x7F0($23)
    vmadh   $v20, $v24, $v31[3]
    vmadh   $v30, $v23, $v31[4]
    vmadh   $v20, $v22, $v31[5]
    vmadh   $v20, $v21, $v31[6]
    vmadh   $v20, $v31, $v18[5]
    lqv     $v31[0], 0x10($23)
    vsar    $v29, $v19, $v30[1]
    vsar    $v30, $v19, $v30[0]
    vmudn   $v20, $v29, $v18[0]
    vmadh   $v30, $v30, $v18[0]
    bgtz    $22, .L_1780
    addi    $23, $23, 0x10
    addi    $1, $23, -0x8
    jal     dma_write
    sqv     $v30[0], 0x7F0($23)
    addi    $30, $30, -0x8
    mfc0    $5, SP_DMA_BUSY
.L_17DC:
    bnez    $5, .L_17DC
    mfc0    $5, SP_DMA_BUSY
    j       cmd_SPNOOP_next_command
    mtc0    $zero, SP_SEMAPHORE

// Command handler for opcode(s) 0x05 RESAMPLE (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_RESAMPLE:
    sll     $2, $26, 8
    vxor    $v23, $v23, $v23
    srl     $2, $2, 8
    addi    $2, $2, 0x0
    addi    $3, $zero, 0xF
    srl     $21, $25, 30
    bgtz    $21, .L_1860
    addi    $1, $24, 0x0
    mfc0    $4, SP_SEMAPHORE
.L_1810:
    bnez    $4, .L_1810
    mfc0    $4, SP_SEMAPHORE
    mfc0    $4, SP_DMA_FULL
.L_181C:
    bnez    $4, .L_181C
    mfc0    $4, SP_DMA_FULL
    mtc0    $1, SP_MEM_ADDR
    mtc0    $2, SP_DRAM_ADDR
    mtc0    $3, SP_RD_LEN
    srl     $20, $25, 2
    andi    $20, $20, 0xFFF
    addi    $20, $20, 0x4E8
    lqv     $v31[0], 0x40($zero)
    lqv     $v25[0], 0x30($zero)
    mfc0    $5, SP_DMA_BUSY
.L_1848:
    bnez    $5, .L_1848
    mfc0    $5, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    ldv     $v19[0], 0x0($24)
    j       .L_187C
    lsv     $v24[14], 0x8($24)
.L_1860:
    srl     $20, $25, 2
    andi    $20, $20, 0xFFF
    addi    $20, $20, 0x4E8
    lqv     $v31[0], 0x40($zero)
    vxor    $v19, $v19, $v19
    lqv     $v25[0], 0x30($zero)
    vxor    $v24, $v24, $v24
.L_187C:
    mtc2    $20, $v21[4]
    addi    $4, $zero, 0xB0
    mtc2    $4, $v21[6]
    vsub    $v25, $v25, $v31
    srl     $4, $25, 14
    mtc2    $4, $v21[8]
    addi    $4, $zero, 0x40
    mtc2    $4, $v21[10]
    vsub    $v25, $v25, $v31
    lqv     $v30[0], 0x50($zero)
    lqv     $v29[0], 0x60($zero)
    lqv     $v28[0], 0x70($zero)
    vmudm   $v24, $v31, $v24[7]
    lqv     $v27[0], 0x80($zero)
    vmadm   $v23, $v25, $v21[4]
    lqv     $v26[0], 0x90($zero)
    vmadn   $v24, $v31, $v30[0]
    sdv     $v19[0], 0x0($20)
    lqv     $v25[0], 0x30($zero)
    vmudn   $v22, $v31, $v21[2]
    addi    $22, $zero, 0x170
    vmadn   $v22, $v23, $v30[2]
    andi    $4, $25, 0x3
    vmudl   $v20, $v24, $v21[5]
    beqz    $4, .L_18E8
    addi    $23, $zero, 0x4F0
    addi    $23, $zero, 0x660
.L_18E8:
    ssv     $v24[7], 0x8($24)
    vmudn   $v20, $v20, $v30[4]
    sqv     $v22[0], 0x7B0($zero)
    vmadn   $v20, $v31, $v21[3]
    sqv     $v20[0], 0x7C0($zero)
    lh      $21, 0xFB0($zero)
    lh      $13, 0xFC0($zero)
    lh      $17, 0xFB8($zero)
    lh      $9, 0xFC8($zero)
    lh      $20, 0xFB2($zero)
    lh      $12, 0xFC2($zero)
    lh      $16, 0xFBA($zero)
    lh      $8, 0xFCA($zero)
    lh      $19, 0xFB4($zero)
    lh      $11, 0xFC4($zero)
    lh      $15, 0xFBC($zero)
    lh      $7, 0xFCC($zero)
    lh      $18, 0xFB6($zero)
    lh      $10, 0xFC6($zero)
    lh      $14, 0xFBE($zero)
    lh      $6, 0xFCE($zero)
.L_193C:
    ldv     $v19[0], 0x0($21)
    vmudm   $v24, $v31, $v24[7]
    ldv     $v18[0], 0x0($13)
    vmadh   $v24, $v31, $v23[7]
    ldv     $v19[8], 0x0($17)
    vmadm   $v23, $v25, $v21[4]
    ldv     $v18[8], 0x0($9)
    vmadn   $v24, $v31, $v30[0]
    ldv     $v17[0], 0x0($20)
    vmudn   $v22, $v31, $v21[2]
    ldv     $v16[0], 0x0($12)
    ldv     $v17[8], 0x0($16)
    vmadn   $v22, $v23, $v30[2]
    ldv     $v16[8], 0x0($8)
    vmudl   $v20, $v24, $v21[5]
    ldv     $v15[0], 0x0($19)
    ldv     $v14[0], 0x0($11)
    ldv     $v15[8], 0x0($15)
    ldv     $v14[8], 0x0($7)
    vmudn   $v20, $v20, $v30[4]
    ldv     $v13[0], 0x0($18)
    vmadn   $v20, $v31, $v21[3]
    ldv     $v12[0], 0x0($10)
    ldv     $v13[8], 0x0($14)
    vmulf   $v11, $v19, $v18
    ldv     $v12[8], 0x0($6)
    vmulf   $v10, $v17, $v16
    sqv     $v22[0], 0x7B0($zero)
    vmulf   $v9, $v15, $v14
    sqv     $v20[0], 0x7C0($zero)
    lh      $21, 0xFB0($zero)
    lh      $13, 0xFC0($zero)
    vmulf   $v8, $v13, $v12
    lh      $17, 0xFB8($zero)
    vadd    $v11, $v11, $v11[1q]
    lh      $9, 0xFC8($zero)
    vadd    $v10, $v10, $v10[1q]
    lh      $20, 0xFB2($zero)
    vadd    $v9, $v9, $v9[1q]
    lh      $12, 0xFC2($zero)
    vadd    $v8, $v8, $v8[1q]
    lh      $16, 0xFBA($zero)
    vadd    $v11, $v11, $v11[2h]
    lh      $8, 0xFCA($zero)
    vadd    $v10, $v10, $v10[2h]
    lh      $19, 0xFB4($zero)
    vadd    $v9, $v9, $v9[2h]
    lh      $11, 0xFC4($zero)
    vadd    $v8, $v8, $v8[2h]
    lh      $15, 0xFBC($zero)
    vmudn   $v7, $v29, $v11[0h]
    lh      $7, 0xFCC($zero)
    vmadn   $v7, $v28, $v10[0h]
    lh      $18, 0xFB6($zero)
    vmadn   $v7, $v27, $v9[0h]
    lh      $10, 0xFC6($zero)
    vmadn   $v7, $v26, $v8[0h]
    lh      $14, 0xFBE($zero)
    lh      $6, 0xFCE($zero)
    addi    $22, $22, -0x10
    blez    $22, .L_1A3C
    sqv     $v7[0], 0x0($23)
    j       .L_193C
    addi    $23, $23, 0x10
.L_1A3C:
    ldv     $v19[0], 0x0($21)
    ssv     $v24[0], 0x8($24)
    jal     dma_write
    sdv     $v19[0], 0x0($24)
    addi    $30, $30, -0x8
    mfc0    $5, SP_DMA_BUSY
.L_1A54:
    bnez    $5, .L_1A54
    mfc0    $5, SP_DMA_BUSY
    j       cmd_SPNOOP_next_command
    mtc0    $zero, SP_SEMAPHORE

// Command handler for opcode(s) 0x03 ENVMIXER (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_ENVMIXER:
    sll     $2, $25, 8
    srl     $2, $2, 8
    addi    $2, $2, 0x0
    lqv     $v31[0], 0x40($zero)
    lqv     $v10[0], 0x50($zero)
    lqv     $v30[0], 0xA0($zero)
    vxor    $v0, $v0, $v0
    srl     $14, $26, 16
    andi    $15, $14, 0x1
    bgtz    $15, .L_1AB8
    addi    $1, $24, 0x0
    jal     dma_read
    addi    $3, $zero, 0x4F
    mfc0    $5, SP_DMA_BUSY
.L_1A9C:
    bnez    $5, .L_1A9C
    mfc0    $5, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    lqv     $v20[0], 0x0($24)
    lqv     $v21[0], 0x10($24)
    lqv     $v18[0], 0x20($24)
    lqv     $v19[0], 0x30($24)
.L_1AB8:
    lqv     $v24[0], 0x40($24)
    addi    $16, $zero, 0x4F0
    addi    $21, $zero, 0x9D0
    addi    $20, $zero, 0xB40
    addi    $19, $zero, 0xCB0
    addi    $18, $zero, 0xE20
    addi    $17, $zero, 0x170
    mfc2    $22, $v24[8]
    beqz    $15, .L_1BA8
    mfc2    $23, $v24[2]
    addi    $3, $zero, 0x4F
    vxor    $v20, $v20, $v20
    lsv     $v20[14], 0x50($24)
    vxor    $v21, $v21, $v21
    lqv     $v17[0], 0x0($16)
    vxor    $v18, $v18, $v18
    mtc2    $26, $v18[14]
    vmudl   $v23, $v30, $v24[2]
    lqv     $v29[0], 0x0($21)
    vmadn   $v23, $v30, $v24[1]
    lqv     $v27[0], 0x0($19)
    vmadh   $v20, $v31, $v20[7]
    lqv     $v28[0], 0x0($20)
    vmadn   $v21, $v31, $v0[0]
    bgez    $23, .L_1B28
    vxor    $v19, $v19, $v19
    j       .L_1B2C
    vge     $v20, $v20, $v24[0]
.L_1B28:
    vlt     $v20, $v20, $v24[0]
.L_1B2C:
    vmudl   $v23, $v30, $v24[5]
    lqv     $v26[0], 0x0($18)
    vmadn   $v23, $v30, $v24[4]
    addi    $17, $17, -0x10
    vmadh   $v18, $v31, $v18[7]
    addi    $16, $16, 0x10
    vmadn   $v19, $v31, $v0[0]
    vmulf   $v16, $v20, $v24[6]
    bgez    $22, .L_1B5C
    vmulf   $v15, $v20, $v24[7]
    j       .L_1B60
    vge     $v18, $v18, $v24[3]
.L_1B5C:
    vlt     $v18, $v18, $v24[3]
.L_1B60:
    vmulf   $v29, $v29, $v10[5]
    vmacf   $v29, $v17, $v16
    vmulf   $v27, $v27, $v10[5]
    vmacf   $v27, $v17, $v15
    vmulf   $v16, $v18, $v24[6]
    vmulf   $v15, $v18, $v24[7]
    sqv     $v29[0], 0x0($21)
    vmulf   $v28, $v28, $v10[5]
    addi    $21, $21, 0x10
    vmacf   $v28, $v17, $v16
    sqv     $v27[0], 0x0($19)
    vmulf   $v26, $v26, $v10[5]
    addi    $19, $19, 0x10
    vmacf   $v26, $v17, $v15
    sqv     $v28[0], 0x0($20)
    addi    $20, $20, 0x10
    sqv     $v26[0], 0x0($18)
    addi    $18, $18, 0x10
.L_1BA8:
    vaddc   $v21, $v21, $v24[2]
    vadd    $v20, $v20, $v24[1]
.L_1BB0:
    lqv     $v29[0], 0x0($21)
    vaddc   $v19, $v19, $v24[5]
    lqv     $v17[0], 0x0($16)
    bgez    $23, .L_1BCC
    vadd    $v18, $v18, $v24[4]
    j       .L_1BD0
    vge     $v20, $v20, $v24[0]
.L_1BCC:
    vlt     $v20, $v20, $v24[0]
.L_1BD0:
    bgez    $22, .L_1BE0
    lqv     $v27[0], 0x0($19)
    j       .L_1BE4
    vge     $v18, $v18, $v24[3]
.L_1BE0:
    vlt     $v18, $v18, $v24[3]
.L_1BE4:
    vmulf   $v16, $v20, $v24[6]
    sqv     $v20[0], 0x0($24)
    vmulf   $v15, $v20, $v24[7]
    sqv     $v21[0], 0x10($24)
    vmulf   $v29, $v29, $v10[5]
    vmacf   $v29, $v17, $v16
    lqv     $v28[0], 0x0($20)
    vmulf   $v27, $v27, $v10[5]
    lqv     $v26[0], 0x0($18)
    vmacf   $v27, $v17, $v15
    addi    $17, $17, -0x10
    vaddc   $v21, $v21, $v24[2]
    addi    $16, $16, 0x10
    vadd    $v20, $v20, $v24[1]
    sqv     $v29[0], 0x0($21)
    vmulf   $v16, $v18, $v24[6]
    addi    $21, $21, 0x10
    vmulf   $v15, $v18, $v24[7]
    sqv     $v27[0], 0x0($19)
    vmulf   $v28, $v28, $v10[5]
    addi    $19, $19, 0x10
    vmacf   $v28, $v17, $v16
    vmulf   $v26, $v26, $v10[5]
    vmacf   $v26, $v17, $v15
    sqv     $v28[0], 0x0($20)
    addi    $20, $20, 0x10
    blez    $17, .L_1C5C
    sqv     $v26[0], 0x0($18)
    j       .L_1BB0
    addi    $18, $18, 0x10
.L_1C5C:
    sqv     $v18[0], 0x20($24)
    sqv     $v19[0], 0x30($24)
    jal     dma_write
    sqv     $v24[0], 0x40($24)
    mfc0    $5, SP_DMA_BUSY
.L_1C70:
    bnez    $5, .L_1C70
    mfc0    $5, SP_DMA_BUSY
    mtc0    $zero, SP_SEMAPHORE
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Command handler for opcode(s) 0x0C MIXER (dispatch-table entry; name inferred from the
// mupen64plus-rsp-hle NAUDIO ABI order, not a Nintendo symbol).
cmd_MIXER:
    lqv     $v31[0], 0x50($zero)
    andi    $22, $25, 0xFFFF
    addi    $22, $22, 0x4F0
    lqv     $v28[0], 0x0($22)
    srl     $23, $25, 16
    addi    $23, $23, 0x4F0
    lqv     $v29[0], 0x0($23)
    mtc2    $26, $v30[0]
    addi    $21, $zero, 0x170
.L_1CA8:
    vmulf   $v27, $v28, $v31[5]
    addi    $21, $21, -0x10
    addi    $23, $23, 0x10
    addi    $22, $22, 0x10
    vmacf   $v27, $v29, $v30[0]
    lqv     $v28[0], 0x0($22)
    lqv     $v29[0], 0x0($23)
    bgtz    $21, .L_1CA8
    sqv     $v27[0], 0x7F0($22)
    j       cmd_SPNOOP_next_command
    addi    $30, $30, -0x8

// Padding: the microcode text object is 0xC60 bytes; the last instruction ends at
// 0x04001CD4, followed by 12 zero bytes that are never executed (no branch, jump or
// dispatch target reaches them).
.fill 12, 0x00

n_aspMainTextEnd:

.close
