#include <stdint.h>

// Let on PA_5
#define LED_PIN         5
#define OUTPUT_MODE     (0x01)

// Peripherals.
#define PERIPH_BASE     0x40000000
#define AHB2PERIPH_BASE (PERIPH_BASE + 0x08000000)

// Clock gating.
#define RCC_BASE        0x40021000
#define RCC_AHBENR      (*(volatile unsigned long*)(RCC_BASE + 0x14))

// GPIOA stuff.
#define GPIOA_BASE      (AHB2PERIPH_BASE + 0x00)
#define GPIOA_MODER     (*(volatile unsigned long*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile unsigned long*)(GPIOA_BASE + 0x14))

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

// Delay a certain number of cycles.
void delay(unsigned int time) {
	while (time--);
}

int main(void){
	  // Enable clock on GPIOA peripheral
    RCC_AHBENR = 1 << 17;

		// Make the port pin be an output.
		GPIOA_MODER |= 0x01 << (LED_PIN * 2);

	while (1) {
		GPIOA_ODR = 1 << LED_PIN;  // set LED pin high
		delay(0x3FFFFF);
		GPIOA_ODR = 0x00;  // set LED pin low
		delay(0x3FFFFF);
	}

	return 0;
}

// Toggle the onboard LED.
void toggle_led(void) {

}
