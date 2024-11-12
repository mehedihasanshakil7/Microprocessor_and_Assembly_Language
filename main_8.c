/*
 * System Clock is set at 8 MHz (HSI) and this clock is provided
 * across all peripherals or devices
 */
#define RCC_BASE            (0x40021000)
#define GPIOC_BASE			(0x40011000)
#define TIM2_BASE			(0x40000000)

#define RCC_APB2ENR			(*(volatile unsigned int *)(RCC_BASE + 0x18))
#define RCC_CR				(*(volatile unsigned int *)(RCC_BASE + 0x00))
#define RCC_APB1ENR			(*(volatile unsigned int *)(RCC_BASE + 0x1C))
#define RCC_CFGR			(*(volatile unsigned int *)(RCC_BASE + 0x04))

#define TIM2_ARR        	(*(volatile unsigned int *)(TIM2_BASE + 0x2C))
#define TIM2_PSC        	(*(volatile unsigned int *)(TIM2_BASE + 0x28))
#define TIM2_SR         	(*(volatile unsigned int *)(TIM2_BASE + 0x10))
#define TIM2_CR1			(*(volatile unsigned int *)(TIM2_BASE + 0x00))

#define GPIOC_CRH			(*(volatile unsigned int *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR			(*(volatile unsigned int *)(GPIOC_BASE + 0x0C))

void delay(int ms);

int main(void) {
	RCC_CFGR &= ~(3 << 0); // Set HSI as SYSCLK (8 MHz)
	while((RCC_CFGR & (3 << 2)) != (0 << 2)); // Wait for HSI to be used as SYSCLK

	RCC_APB2ENR |= (1 << 4); // Enable the clock of GPIOC
	RCC_APB1ENR |= (1 << 0); // Enable the clock of TIM2

	// Configure GPIOC pin 13 for general purpose output push-pull at 2 MHz
	GPIOC_CRH &= ~(3 << 20);
	GPIOC_CRH |= (1 << 21);
	GPIOC_CRH &= ~(3 << 22);

	// Blink the LED for specified time
	while(1) {
		GPIOC_ODR |= (1 << 13);
		delay(1000); //milliseconds

		GPIOC_ODR &= ~(1 << 13);
		delay(1000); //milliseconds
	}
}

void delay(int ms) {
	TIM2_PSC = 8000 - 1; // TIM2 running at 1000 Hz
	TIM2_ARR = ms - 1; // TIM2 will count up to this value
	TIM2_SR &= ~(1 << 0); // Clear update flag
	TIM2_CR1 |= (1 << 0); // Enable TIM2
	while(!(TIM2_SR & 0x01)); // Wait for TIM2 to finish counting
	TIM2_CR1 &= ~(1 << 0); // Disable TIM2
}
