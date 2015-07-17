#include <stdint.h>

// Let on PA_5
#define LED_PIN         5

// Peripherals.
#define PERIPH_BASE     0x40000000
#define AHB2PERIPH_BASE (PERIPH_BASE + 0x08000000)

// Clock gating.
#define RCC_BASE        0x40021000
#define RCC_CR          (*(volatile unsigned long*)(RCC_BASE + 0x00))
#define RCC_CFGR        (*(volatile unsigned long*)(RCC_BASE + 0x04))
#define RCC_CFGR2       (*(volatile unsigned long*)(RCC_BASE + 0x2C))
#define RCC_AHBENR      (*(volatile unsigned long*)(RCC_BASE + 0x14))

#define external_osc    8000000

// PLL settings
#define PLL_input_clock_x_4       0b0010
#define PLL_input_clock_x_5       0b0011
#define PLL_input_clock_x_6       0b0100
#define PLL_input_clock_x_7       0b0101
#define PLL_input_clock_x_8       0b0110
#define PLL_input_clock_x_9       0b0111

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

// Delay a certain number of cycles using glorious inline assembly.
void delay(uint32_t time) {
	asm volatile(
		"mov r4, #3                \n" // Divide time by three since the loop is
		"udiv %[time], %[time], r4 \n" // 3x too slow.
		"loop:                     \n"
		"subs %[time], %[time], #1 \n" // 1 cycle
		"bne loop                  \n" // 1 cycle if not take, 2 if taken.
		: [time] "+l" (time)           // Put rw input variable time in r0..r7.
																   // Make it rw and as output so we don't clobber
		:                              // Time is both input and output.
		: "r4", "cc"                   // We are clobbering r4 and condition code flags.
	);
}

// Enables the external oscillator and PLL to get 72 Mhz full speed arm core,
// and GPIO Port A clock.
void clocks_init(void){
	// Turn on the High Speed External oscillator.
	RCC_CR = RCC_CR | (1 << 16);

	// Set PREDIV1SRC to external oscillator.
  RCC_CFGR2 = RCC_CFGR2 | (0 << 16);

	// Set prediv1 to /1.
  RCC_CFGR2 = RCC_CFGR2 | (0b0000 << 0);

	// Set PLLSRC to be from clock divider 1.
	RCC_CFGR = RCC_CFGR | (1 << 16);

	// Set the PLLMUL to be x9 (8 Mhz * 4 = 32 Mhz)
	RCC_CFGR = RCC_CFGR | (PLL_input_clock_x_4 << 18);

	// Turn on the PLL.
	RCC_CR = RCC_CR | (1 << 24);

	// Have the MCO pin output PLLCLK/2
	// RCC_CFGR = RCC_CFGR | (0b0111 << 24);

  // Wait a bit for clocks to stabalize.
	delay(0xFFF);

	// Select PLLCLK for SW leading to the sysclk.
	RCC_CFGR = RCC_CFGR | 0b10;

	// Enable clock on GPIOA peripheral
	RCC_AHBENR = 1 << 17;
}

int main(void){
	// Setup all our necessary clocks.
	clocks_init();

	// What 1 second worth of cycles represents.
	const uint32_t second_cycles = external_osc * 4;

	// Make the port pin be an output.
	GPIOA_MODER |= 0x01 << (LED_PIN * 2);

	// Blink the led with a period of 1 second.
	while (1) {
		GPIOA_ODR = 1 << LED_PIN;  // set LED pin high
		delay(second_cycles / 2);
		GPIOA_ODR = 0x00;  // set LED pin low
		delay(second_cycles / 2);
	}

	return 0;
}
