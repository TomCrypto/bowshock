.text
.balign 2
.syntax unified
.thumb
.global rtl_init

.thumb_func
MapRegion:
    b       2f
1:
    ldmia   r0!, {r3}
    stmia   r1!, {r3}
2:
    cmp     r1, r2
    bne     1b

    bx lr

.thumb_func
ZeroRegion:
    movs    r2, #0

    b       2f
1:
    stmia   r0!, {r2}
2:
    cmp     r0, r1
    bne     1b

    bx lr

.thumb_func
rtl_init:
    cpsid   i

    @ Initialize the main stack pointer.

    ldr     r0, =__LD_STACK_TOP
    msr     MSP, r0
    isb

    @ TODO: use the main stack pointer for thread mode for now.
    @ Later on, use a separate PSP (process stack pointer)!

    ldr     r0, 0
    msr     CONTROL, r0
    isb

    @ Map the data segment from ROM into RAM.

    ldr     r0, =__LD_DATA_POS
    ldr     r1, =__LD_DATA_OFF
    ldr     r2, =__LD_DATA_END
    bl      MapRegion

    @ Zero-initialize the BSS region.

    ldr     r0, =__LD_BSS_OFF
    ldr     r1, =__LD_BSS_END
    bl      ZeroRegion

    @ Jump to the kernel initialization main routine.

    cpsie   i
    bl      rtl_startup
.loop:
    b       .loop
