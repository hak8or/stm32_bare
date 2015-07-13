/* Memory layout for an STM32F103RB */
MEMORY
{
   FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
   RAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

STACK_SIZE = 1024;
_estack = ORIGIN(RAM) + LENGTH(RAM);
_sstack = _estack - STACK_SIZE;

/* Output sections */
SECTIONS
{
    .text :
    {
        /* ISR Vector Table first */
        _isr_vectors_offs = . - 0x08000000;
        KEEP(*(.isr_vector))

        *(.text)        /* Program code */
        *(.text.*)
        *(.rodata)      /* Constants */
        *(.fini)
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)
        _start_data_wrapper_flash = .;
    } > FLASH

    /* Initialized global and static variables */
    .data :
    {
        start_data = .;
        *(.data)
        *(.data.*)
        end_data = .;

    } > RAM AT> FLASH

    .data_init :
    {
      _end_data_wrapper_flash = .;
    } > FLASH

    /* Uninitialized global and static variables */
    .bss :
    {
        start_bss = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        end_bss = .;

    } > RAM

    /* Stack */
    . = _sstack;
    .stack :
    {
        . = . + STACK_SIZE;
    } > RAM
}
