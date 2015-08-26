.set trace_base,  0x10000001
.set podr_a, 0xEC094000
.set podr_e, 0xEC094004
.set dsdr_b, 0xEC094019
.set dsdr_h, 0xEC09401F
.set dsdr_e, 0xEC09401C
.set clrr_h, 0xEC09402B
.set clrr_e, 0xEC094028
.set dspi1, 0xFC03C000
.set dspi1_sr, dspi1+0x2C
.set dspi1_pushr, dspi1+0x34
.set dspi_tfff, 0x02000000
.set dspi_tx_mask, 0x80010000
.set dspi_tx_done_mask, 0x00010000
.set rgpio, 0x8C000000
.set rgpio_clr, sim_rgpio+0x06
.set rgpio_set, sim_rgpio+0x0A
.set rgpio_tog, sim_rgpio+0x0E

.macro tfff_loop
1:
    move.l dspi_sr, %d0
    andi.l dspi_tfff, %d0
    bne 1b
.endm

.macro send_data dat mask
    move.l \dat, %d0
    lsl     #16, %d0
    ori.l  \mask, %d0
    tfff_loop
    move.l  %d0, dspi1_pushr
    move.w \dat, %d0 ; move word to get the low 16 without masking
    ori.l  \mask, %d0
    tfff_loop
    move.l  %d0, dspi1_pushr
.endm

.macro nop_loop cnt
    move.l \cnt, %d0
1:
    nop
    subi.l #1, %d0
    bne 1b  
.endm

.macro starting_trace
|    move.b  podr_e, %d0
|    andi.l  #0x7F, %d0
|    move.b  %d0, podr_e
|    nop
|    nop
.endm

.macro ending_trace
|    clr.l %d0
|    move.b trace_frame_count, %d0
|    subi.l #1, %d0
|    bne 3f
|    move.b  #0xEF, %d0
|    move.b  %d0, clrr_h
|    nop_loop #8
|    move.b  #0x10, %d0
|    move.b  %d0, dsdr_h
|    nop_loop #30
|    move.b #15, %d0
|3:
|    move.b %d0, trace_frame_count
.endm

.align(0x10)
wait_for_space:
    clr.l %d0
    nop_loop #4
2:
    move.b dsdr_b, %d0
    btst #4, %d0
    beq 2b
    rts

.align(0x10)
.global trace_exception
trace_exception:
    lea     -16(%sp), %sp
    movem.l %d0-%d1/%a0-%a1, (%a7)
    
    # set up to write to FB
    starting_trace
    move.l  #0x10000000, %d0
    lea     +20(%sp), %a1

    # write PC
    move.l  %a1@, %d1
    move.l  %d0, %a0

    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space

    move.w  %d1, (%a0)
    move.l  %a1@, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write d0
    lea     +16(%sp), %a1
    move.l  %a1@, %d1

    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a1@, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)
    # write d1
    lea     +12(%sp), %a1
    move.l  %a1@, %d1

    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a1@, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    #movem.l %d2-%d7, (%a0)

    # write d2
    move.l  %d2, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d2, (%a0)

    # write d3
    move.l  %d3, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d3, (%a0)

    # write d4
    move.l  %d4, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d4, (%a0)

    # write d5
    move.l  %d5, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d5, (%a0)

    # write d6
    move.l  %d6, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d6, (%a0)

    # write d7
    move.l  %d6, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    jsr wait_for_space
    move.w  %d7, (%a0)

    # write a0
    lea     +8(%sp), %a1
    move.l  (%a1), %d1

    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  (%a1), %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a1
    lea     +4(%sp), %a1
    move.l  (%a1), %d1

    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  (%a1), %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    #movem.l %a2-%a7, (%a0)
    # write a2
    move.l  %a2, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a2, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a3
    move.l  %a3, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a3, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a4
    move.l  %a4, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a4, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a5
    move.l  %a5, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a5, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a6
    move.l  %a6, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a6, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    # write a7
    move.l  %a7, %d1
    lsr.l   #8, %d1
    lsr.l   #8, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    move.l  %a7, %d1
    jsr wait_for_space
    move.w  %d1, (%a0)

    ending_trace
    movem.l (%a7), %d0-%d1/%a0-%a1
    lea     16(%a7), %a7

    rte

