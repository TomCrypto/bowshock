OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);

MEMORY
{
        rom (rx)        : org = 0x00000000, len = 16k
        ram (rwx)       : org = 0x10000000, len = 4k
}

ENTRY(rtl_init);

__LD_STACK_LEN = 1024;

SECTIONS
{
        /* Flash layout */

        .vect : {
                . = ALIGN(4);
                KEEP(*(.vect));
                . = ALIGN(4);
        } > rom AT > rom

        .text : {
                . = ALIGN(4);
                *(.text .text.*);
                . = ALIGN(4);
                *(.rodata .rodata.*);
                . = ALIGN(4);
        } > rom AT > rom

        /* Memory layout */

        .stack (NOLOAD) : {
                . += __LD_STACK_LEN;
                . = ALIGN(8);
                __LD_STACK_TOP = .;
        } > ram AT > ram

        .bss (NOLOAD) : {
                . = ALIGN(4);
                __LD_BSS_OFF = .;
                *(.bss .bss.*);
                *(COMMON);
                . = ALIGN(4);
                __LD_BSS_END = .;
        } > ram AT > ram

        .data : {
                . = ALIGN(4);
                __LD_DATA_OFF = .;
                *(.data .data.*);
                . = ALIGN(4);
                __LD_DATA_END = .;
        } > ram AT > rom

        .nvram : {
                . = ALIGN(4);
                *(.nvram .nvram.*);
                . = ALIGN(4);
        } > ram AT > rom

        /* Reserved for the IAP routine's state */

        .reserved_iap ORIGIN(ram) + LENGTH(ram) - 0x20 : {
                . += 0x20;
        } > ram AT > ram
}

/* Provide stack, data and fast code boundaries */

PROVIDE(__LD_STACK_OFF = 0);
PROVIDE(__LD_STACK_TOP = __LD_STACK_TOP);

PROVIDE(__LD_BSS_OFF = __LD_BSS_OFF);
PROVIDE(__LD_BSS_END = __LD_BSS_END);

PROVIDE(__LD_DATA_OFF = __LD_DATA_OFF);
PROVIDE(__LD_DATA_END = __LD_DATA_END);
PROVIDE(__LD_DATA_POS = LOADADDR(.data));

/* Provide flash boundaries for IAP */

PROVIDE(__LD_ROM_OFF = ORIGIN(rom));
PROVIDE(__LD_ROM_LEN = LENGTH(rom));
