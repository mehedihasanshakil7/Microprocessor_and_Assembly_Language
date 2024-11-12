/*
 * Here SYSCLK is 24 MHz, APB1 prescaler is 8, so TIM2 will run at (3x2) 6 MHz
 * This code must be compile with STM32CubeIDE
 * keil may show erroneous behavior
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
	RCC_CR &= ~(1 << 24); // Disable the PLL, necessary to configure it at RCC_CFGR
	RCC_CR &= ~(1 << 0); // Also disable HSI

	// Set PLL multiplication factor at 6 (0100)
	RCC_CFGR &= ~(15 << 18);
	RCC_CFGR |= (1 << 20);

	// Set PLL as SYSCLK
	RCC_CFGR &= ~(3 << 0);
	RCC_CFGR |= (1 << 1);

	// Set HSI/2 as PLL source
	RCC_CFGR &= ~(1 << 16);

	// APB1 prescaler = 8 (110)
	RCC_CFGR &= ~(7 << 8);
	RCC_CFGR |= (3 << 9);

	RCC_CR |= (1 << 0); // Enable HSI
	RCC_CR |= (1 << 24); // Enable PLL
	while((RCC_CFGR & (1 << 3)) != 0);

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
	TIM2_PSC = 6000 - 1; // TIM2 running at 1000 Hz
	TIM2_ARR = ms - 1; // TIM2 will count up to this value
	TIM2_SR &= ~(1 << 0); // Clear update flag
	TIM2_CR1 |= (1 << 0); // Enable TIM2
	while(!(TIM2_SR & 0x01)); // Wait for TIM2 to finish counting
	TIM2_CR1 &= ~(1 << 0); // Disable TIM2
}
