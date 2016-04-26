.global blink
.global blink_end
blink:
    @ Configure clock gate to turn on GPIO port B
    ldr     r0, sim_scgc5
    mov     r1, #0x80
    lsl     r1, #3
    ldr     r2, [r0, #0]
    orr     r1, r2
    str     r1, [r0, #0]

    @ Configure PORT B control regs for 10, 11, 13 (LED pins)
    ldr     r0, portb_pc10
    ldr     r1, [r0, #0]
    mov     r2, #0x80
    lsl     r2, #1
    orr     r1, r2
    str     r1, [r0, #0]    @ set PORTB_PCR10
    str     r1, [r0, #4]    @ set PORTB_PCR11
    str     r1, [r0, #0xC]  @ set PORTB_PCR13
    
    @ Configure LED pins as Output
    ldr     r0, gpiob_pdor
    ldr     r1, [r0, #0x14] @ PDDR is +0x14 from PDOR
    mov     r2, #0xB0
    lsl     r2, #6
    str     r2, [r0, #0x4] @ Turn off LEDs (PSOR is +0x4 from PDOR)
    orr     r1, r2
    str     r1, [r0, #0x14]
    
    @ Prep the bit set register
    @ Red LED is    gpiob_10
    @ Green LED is  gpiob_11
    @ Blue LED is   gpiob_13
    mov     r1, #0x80
    lsl     r1, #3 @ bit 10 is now set

blink_outer_loop:
    str     r1, [r0, #0x8]  @ PTOR is +0xC from PDOR
    bl      dly
    str     r1, [r0, #0x4]  @ PTOR is +0xC from PDOR
    bl      dly

    lsl     r1, #1  @ bit 11 is now set
    str     r1, [r0, #0x8]  @ PTOR is +0xC from PDOR
    bl      dly
    str     r1, [r0, #0x4]  @ PTOR is +0xC from PDOR
    bl      dly

    lsl     r1, #2  @ bit 13 is now set
    str     r1, [r0, #0x8]  @ PTOR is +0xC from PDOR
    bl      dly
    str     r1, [r0, #0x4]  @ PTOR is +0xC from PDOR
    bl      dly

    lsr     r1, #3 @ bit 10 is now set
    b       blink_outer_loop

dly:
    ldr     r5, dly_cnt
    mov 	r4, #0
dly_loop:
    sub     r5, #1
    cmp 	r5, r4
    bne     dly_loop
    mov     pc, lr          @ Return from dly

.align(4)
sim_scgc5:
    .word   0x40048038
portb_pc10:
    .word   0x4004A028
gpiob_pdor:
    .word   0x400FF040
dly_cnt:
    .word   50000

blink_end:
