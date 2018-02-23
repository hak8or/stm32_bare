/* Interrupt vector table. */
.cpu cortex-m3
.thumb
.word   _estack      /* stack top address  */
.word   reset        /* Reset handler      */
.word   hang         /* NMI handler        */
.word   hang         /* Hardfault handler  */
.word   hang         /* Memfault  handler  */
.word   hang         /* Busfault handler   */
.word   hang         /* Undefined ins      */

/*
 * Remaining interrupts are not used and therefore not included,
 * so it's fine if we overwrite their area with instructions.
 */

/* Function defenition for starting up */
.thumb_func
reset:
  bl low_level_init
  bl main
  b hang

/* Function defenition for hanging */
.thumb_func
hang:
	b .
