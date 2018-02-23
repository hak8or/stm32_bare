// Include our vendor provided STM32 header.
extern "C" {
	#include "src/vendor/stm32f30x.h"
}

// All symbols defined in the linkerfile for
// getting a enviorment up according to C standard.
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

// Delay a certain number of cycles using glorious inline assembly.
void delay(uint32_t time) {
	asm volatile(
		"mov r4, #3                \n" // Divide time by three since the loop is
		"udiv %[time], %[time], r4 \n" // 3x too slow.
		"loop:                     \n"
		"subs %[time], %[time], #1 \n" // 1 cycle
		"bne loop                  \n" // 1 cycle if not taken, 2 if taken.
		: [time] "+l" (time)           // Put rw input variable time in r0..r7.
		                               // Make it rw and as output so we don't clobber
		:                              // Time is both input and output.
		: "r4", "cc"                   // We are clobbering r4 and condition code flags.
	);
}

// Enables the external oscillator and PLL while configuring
// the dividers so we get 72 Mhz.
void clocks_init(void){
	// Turn on the High Speed External oscillator.
	RCC->CR = RCC->CR | (1 << 16);

	// Set PREDIV1SRC to external oscillator.
	RCC->CFGR2 = RCC->CFGR2 | (0 << 16);

	// Set prediv1 to /0 (don't divide).
	RCC->CFGR2 = RCC->CFGR2 | (0 << 0);

	// Set PLLSRC to be from prediv1.
	RCC->CFGR = RCC->CFGR | (1 << 16);

	// Set the PLLMUL to be x9 (8 Mhz * 9 = 72 Mhz).
	RCC->CFGR = RCC->CFGR | RCC_CFGR_PLLMULL9;

	// Turn on the PLL.
	RCC->CR = RCC->CR | (1 << 24);

	// Set the APB1 prescaler to /2.
	// 72Mhz / 2 = 36 Mhz, with 36 Mhz being max
	RCC->CFGR = RCC->CFGR | (RCC_CFGR_PPRE1_DIV2);

	// Set the AHB prescaler to /0.
	RCC->CFGR = RCC->CFGR | (RCC_CFGR_HPRE_DIV1);

	// Change the flash wait states to 2.
	FLASH->ACR = FLASH->ACR | (0b011 << 0);

	// Wait a bit for clocks to stabalize.
	delay(0xFF);

	// Select PLLCLK for SW leading to the sysclk.
	RCC->CFGR = RCC->CFGR | 0b10;

	// Disable internal oscillator
	RCC->CR = RCC->CR & 0xFFFFFFFE;

	// Enable clock on GPIOA peripheral
	RCC->AHBENR = RCC->AHBENR | (1 << 17);
}

int main(void){
	// Setup all our necessary clocks.
	clocks_init();

	// What 1 second worth of cycles represents.
	const uint32_t second_cycles = 72'000'000;

	// Led is represented by BIT5  in the GPIO register.
	const uint32_t LED_BIT = 5;

	// Make the port pin be an output.
	GPIOA->MODER |= 0x01 << (LED_BIT * 2);

	// Blink the led with a period of 1 second.
	while (1) {
		// Set LED pin.
		GPIOA->BSRR = 1 << LED_BIT;
		delay(second_cycles / 2);

		// Reset LED pin
		GPIOA->BSRR = 1 << (LED_BIT + 16);
		delay(second_cycles / 2);
	}

	return 0;
}
