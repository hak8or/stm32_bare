#include <stdint.h>

unsigned int some_global_var = 5;
unsigned int some_global_var2 = 0;

extern uint32_t start_bss;
extern uint32_t end_bss;

extern uint32_t start_data;
extern uint32_t end_data;
extern uint32_t _start_data_wrapper_flash;
extern uint32_t _end_data_wrapper_flash;

// So the linker sees this.
extern void low_level_init(void) __asm__("low_level_init");

// Sets up our variables.
void low_level_init(void){
	// Where our non zero variables will be stored in RAM.
	uint32_t* data_ptr   = &start_data;
  uint32_t* data_end   = &end_data;

	// Where the values of our variables are stored in FLASH and therefore where
	// we are copying from.
	uint32_t* data_in_flash_ptr = &_start_data_wrapper_flash + 2;
	uint32_t* data_in_flash_end = &_end_data_wrapper_flash;

	// And copy the variable contents from flash to ram.
	while (data_ptr < data_end) {
		*data_ptr = *data_in_flash_ptr;
		data_ptr++;
		data_in_flash_ptr++;
	}

	// Set our zero initlized variables to actually be zero.
	uint32_t* bss_ptr   = &start_bss;
	uint32_t* bss_end   = &end_bss;
	while (bss_ptr < bss_end) {
		*bss_ptr = 0;
		bss_ptr++;
	}
}

int main(void){
	volatile int bar = 3;

	int foo = bar;
	foo = foo << 1;
	foo = foo + (foo * 2);
	foo = foo * foo;

	bar = foo;

	if (some_global_var == 0) {
		while(1){};
	}

	while(1){
		int i = 5;
		i = i + i;
	}

	return 0;
}

// Delay a certain number of cycles.
void delay(unsigned int time) {
	while (time--);
}

// Toggle the onboard LED.
void toggle_led(void) {

}
