/* vectors.s */
.cpu cortex-m3
.thumb

.word    0x20010000  /* stack top address  */
.word   _start       /* Reset handler      */
.word   hang         /* NMI handler        */
.word   hang         /* Hardfault handler  */
.word   hang         /* Memfault  handler  */
.word   hang         /* Busfault handler   */
.word   hang         /* Undefined ins      */
/*
 * Remaining interrupts are not used and therefore not included,
 * if needed add them here.
 */

.thumb_func
.global _start
_start:
	bl main
	b hang

.thumb_func
hang:
	b .
