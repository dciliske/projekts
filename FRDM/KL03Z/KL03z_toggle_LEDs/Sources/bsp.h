#ifndef __BSP_H
#define __BSP_H

#define __disable_irq() __asm__ volatile("CPSID i":::"memory");
#define __enable_irq()	__asm__ volatile("CPSIE i":::"memory");

#define PORT_FUNC_GPIO_W_DSE 	0x0140
#define PORT_FUNC_SPI 			0x0300
#define PORT_FUNC_UART_TX_RX 	0x0200

#define SER_RX_BUF_LEN 80
#define UART_TX_PIN 1
#define UART_RX_PIN 2
#define SWD_CLK_PIN 9
#define SWD_IO_PIN 8

void config_clocks();
void config_spi();
void config_uart();

void putachar(char c);
void putstring(const char *str);
void newline();
void putbyte(uint8_t b);
void putword(uint32_t w);
bool charavail();
uint8_t getachar();

#ifdef _DEBUG
#define dbputstring(x) putstring(x)
#define dbnewline() newline()
#define dbputbyte(x) putbyte(x)
#define dbputword(x) putword(x)
#else
#define dbputstring(x)
#define dbnewline()
#define dbputbyte(x)
#define dbputword(x)
#endif


#endif /* #ifndef __BSP_H */
