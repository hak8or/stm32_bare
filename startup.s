/* vectors.s */
.cpu cortex-m3
.thumb

.word   0x20002000  /* stack top address */
.word   _start
.word   hang
.word   hang
.word   hang
.word   hang
.word   hang

.thumb_func
.global _start
_start:
	bl main
	b hang

.thumb_func
hang:
	b .
