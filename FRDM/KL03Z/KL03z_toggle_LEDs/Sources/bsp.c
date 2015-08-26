#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"
#include "core_cm0plus.h"
#include "MKL03Z4.h"

static volatile uint8_t RxBuf[SER_RX_BUF_LEN];
static volatile uint8_t RxDatHead = 0;
static volatile uint8_t RxDatTail = 0;
static volatile uint8_t RxDatCount = 0;


void config_clocks()
{
	SIM->SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);
	SIM->CLKDIV1 = 1 << 16;
	MCG_C1 = 0x02;

	for (volatile uint16_t i = 0; i < 10000; i++) {}
}

void config_spi()
{
	SPI0_BR = 0x01;
	SPI0_C1 = 0x51; // config for Least Significant Bit First
	SPI0_C2 = 0x0B;
	volatile uint8_t j = SPI0_D;
	j = SPI0_S;
}

void config_uart()
{
	SIM->SOPT2  |= 1 << 26;
	SIM->SCGC5 |= 1 << 20;
	LPUART0_BAUD = 0x1F00001A; // 57600 Baud
	LPUART0_CTRL = 0x002C0000;
	PORTB_GPCLR = (((1 << UART_TX_PIN) | (1 << UART_RX_PIN)) << 16)
					| PORT_FUNC_UART_TX_RX;
	NVIC_EnableIRQ(LPUART0_IRQn);
}

void putachar(char c)
{
	while ((LPUART0_STAT&LPUART_STAT_TDRE_MASK) == 0) {}
	LPUART0_DATA = c;
}

 void putstring(const char *str)
{
	while (*str) { putachar(*str++); }
}

void newline()
{
//	putachar('\r');
	putachar('\n');
}

void putbyte(uint8_t b)
{
	if ((b >> 4) < 0xA) { putachar('0'+(b>>4)); }
	else 				{ putachar('7'+(b>>4)); }

	if ((b&0xF) < 0xA) 	{ putachar('0'+(b&0xF)); }
	else 				{ putachar('7'+(b&0xF)); }
}

void putword(uint32_t w)
{
	putbyte(((uint8_t *)&w)[3]);
	putbyte(((uint8_t *)&w)[2]);
	putbyte(((uint8_t *)&w)[1]);
	putbyte(((uint8_t *)&w)[0]);
}

bool charavail()
{
	return RxDatHead != RxDatTail;
}

uint8_t getachar()
{
	uint8_t c;
	while (RxDatCount==0) { }

	__disable_irq();
	c = RxBuf[RxDatTail];

	if (RxDatTail != RxDatHead) {
		if (RxDatTail == (SER_RX_BUF_LEN - 1)) 	{ RxDatTail = 0; }
		else 									{ RxDatTail++; }
	}
	RxDatCount--;

	__enable_irq();
	return c;
}

void LPUART0_IRQHandler()
{
	__disable_irq();
	GPIOB->PTOR = 1 << 10;
	uint8_t dat = LPUART0_DATA;
	uint8_t index;
	if (RxDatHead == RxDatTail)	{ index = RxDatHead;}
	else 						{ index = RxDatHead + 1; }

	if (index >= SER_RX_BUF_LEN) { index = 0; }

	if (RxDatCount < SER_RX_BUF_LEN) {
		RxBuf[index] = dat;
		RxDatHead = index;
		RxDatCount++;
	}
	GPIOB->PTOR = 1 << 10;
	__enable_irq();
}
