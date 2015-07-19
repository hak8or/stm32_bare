#include <stdint.h>

// Let on PA_5
#define LED_PIN         5

// GPIO stuff.
#define GPIOA_BASE      0x48000000
#define GPIOA_MODER     (*(volatile unsigned long*)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile unsigned long*)(GPIOA_BASE + 0x14))
#define GPIOA_BSRR      (*(volatile unsigned long*)(GPIOA_BASE + 0x18))

// Clock gating.
#define RCC_BASE        0x40021000
#define RCC_CR          (*(volatile unsigned long*)(RCC_BASE + 0x00))
#define RCC_CFGR        (*(volatile unsigned long*)(RCC_BASE + 0x04))
#define RCC_CFGR2       (*(volatile unsigned long*)(RCC_BASE + 0x2C))
#define RCC_AHBENR      (*(volatile unsigned long*)(RCC_BASE + 0x14))
#define RCC_AHB2ENR     (*(volatile unsigned long*)(RCC_BASE + 0x18))

#define external_osc    8000000

// PLL settings
#define PLL_input_clock_x_2       0b0000
#define PLL_input_clock_x_3       0b0001
#define PLL_input_clock_x_4       0b0010
#define PLL_input_clock_x_5       0b0011
#define PLL_input_clock_x_6       0b0100
#define PLL_input_clock_x_7       0b0101
#define PLL_input_clock_x_8       0b0110
#define PLL_input_clock_x_9       0b0111
#define PLL_input_clock_x_10      0b1000
#define PLL_input_clock_x_11      0b1001
#define PLL_input_clock_x_12      0b1010
#define PLL_input_clock_x_13      0b1011
#define PLL_input_clock_x_14      0b1100
#define PLL_input_clock_x_15      0b1101
#define PLL_input_clock_x_16      0b1110

// AHB prescaler settings.
#define SYSCLK_div_0   0b0000
#define SYSCLK_div_2   0b1000
#define SYSCLK_div_4   0b1001
#define SYSCLK_div_8   0b1010
#define SYSCLK_div_16  0b1011
#define SYSCLK_div_64  0b1100
#define SYSCLK_div_128 0b1101
#define SYSCLK_div_256 0b1110
#define SYSCLK_div_512 0b1111

// PREDIV division factor for HSE input to PLL.
#define PREDIV_0      0b0000
#define PREDIV_2      0b0001
#define PREDIV_3      0b0010
#define PREDIV_4      0b0011
#define PREDIV_5      0b0100
#define PREDIV_6      0b0101
#define PREDIV_7      0b0110
#define PREDIV_8      0b0111
#define PREDIV_9      0b1000
#define PREDIV_10     0b1001
#define PREDIV_11     0b1010
#define PREDIV_12     0b1011
#define PREDIV_13     0b1100
#define PREDIV_14     0b1101
#define PREDIV_15     0b1110
#define PREDIV_16     0b1111

// APB Low-speed prescaler (APB1) divider.
#define PPRE1_div_0  0b000
#define PPRE1_div_2  0b100
#define PPRE1_div_4  0b101
#define PPRE1_div_8  0b110
#define PPRE1_div_16 0b111

// Flash controller registers.
#define FLASH_BASE      0x40022000
#define FLASH_ACR       (*(volatile unsigned long*)(FLASH_BASE + 0x00))

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

	// Set prediv1 to /0 (don't divide).
	RCC_CFGR2 = RCC_CFGR2 | (PREDIV_0 << 0);

	// Set PLLSRC to be from prediv1.
	RCC_CFGR = RCC_CFGR | (1 << 16);

	// Set the PLLMUL to be x9 (8 Mhz * 9 = 72 Mhz).
	RCC_CFGR = RCC_CFGR | (PLL_input_clock_x_9 << 18);

	// Turn on the PLL.
	RCC_CR = RCC_CR | (1 << 24);

	// Set the APB1 prescaler to /2.
	// 72Mhz / 2 = 36 Mhz, with 36 Mhz being max
	RCC_CFGR = RCC_CFGR | (PPRE1_div_2 << 8);

	// Set the AHB prescaler to /0.
	RCC_CFGR = RCC_CFGR | (SYSCLK_div_0 << 4);

	// Change the flash wait states to 2.
	volatile uint32_t foo1 = FLASH_ACR;
	FLASH_ACR = FLASH_ACR | (0b011 << 0);
	volatile uint32_t foo2 = FLASH_ACR;

	// Wait a bit for clocks to stabalize.
	delay(0xFF);

	// Select PLLCLK for SW leading to the sysclk.
	RCC_CFGR = RCC_CFGR | 0b10;

	// Disable internal oscillator
	RCC_CR = RCC_CR & 0xFFFFFFFE;

	// Enable clock on GPIOA peripheral
	RCC_AHBENR = RCC_AHBENR | (1 << 17);
}

int main(void){
	// Setup all our necessary clocks.
	clocks_init();

	// What 1 second worth of cycles represents.
	const uint32_t second_cycles = external_osc * 9;

	// Make the port pin be an output.
	GPIOA_MODER |= 0x01 << (LED_PIN * 2);

	// Blink the led with a period of 1 second.
	while (1) {
		// Set LED pin.
		GPIOA_BSRR = 1 << LED_PIN;
		delay(second_cycles / 2);

		// Reset LED pin
		GPIOA_BSRR = 1 << (LED_PIN + 16);
		delay(second_cycles / 2);
	}

	return 0;
}
