/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include "swd.h"
#include "bsp.h"
#include "swd_kl03.h"

//#define _DEBUG

#define delay 500000
#define SRAM_START (0x1FFFFE00)
//#define SWD_CLK_PIN 9
//#define SWD_IO_PIN 8
//#define WRITE_DLY 4
//#define UART_TX_PIN 1
//#define UART_RX_PIN 2
//
//
//#define PORT_FUNC_GPIO_W_DSE 	0x0140
//#define PORT_FUNC_SPI 			0x0300
//#define PORT_FUNC_UART_TX_RX 	0x0200
//
//#define SWD_MAX_FAIL 50
//
//
//#define CLK_TRSTN GPIOA_PTOR = (1 << SWD_CLK_PIN)
//#define CLK_CYCLE CLK_TRSTN; __asm("DSB"); CLK_TRSTN;
//#define DIO_STATE ((GPIOA_PDIR & (1 << SWD_IO_PIN)) >> SWD_IO_PIN)
//#define DIO_HI 		GPIOA_PSOR = 1 << SWD_IO_PIN;
//#define DIO_LO		GPIOA_PCOR = 1 << SWD_IO_PIN;
//
//#define SWD_AP_NDP 	0x02
//#define SWD_RNW 	0x04
//#define SWD_START  	0x01
//#define SWD_PARK 	0x80

//const uint8_t BusResetData[7] = {
//		0xFF, 0xFF, 0xFF, 0xFF,
//		0xFF, 0xFF, 0xFF
//};
//
//const uint8_t SWD_change_seq[2] = {0xE7, 0x9E}; // the magic sequence
//
//const uint8_t SWDParityTable[16] = {
//		0x00, 0x01, 0x01, 0x00,
//		0x01, 0x00, 0x00, 0x01,
//		0x01, 0x00, 0x00, 0x01,
//		0x00, 0x01, 0x01, 0x00,
//};

extern const unsigned long blink_code_len;
extern const unsigned char blink_code[];

#ifdef _DEBUG
void putstring(const char *str);

void newline();

void putbyte(uint8_t b);
void putword(uint32_t b);

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

//
//void write_bytes(const uint8_t *bytes, int count)
//{
//	int i;
//	volatile int j;
//
////	GPIOA_PDDR |= (1 << SWD_IO_PIN);
////	for (i = 0; i < count; i++) {
////		for (j = 0; j < 8; j++) {
////			if ((bytes[0] >> j) & 0x1) 	{ DIO_HI; }
////			else 						{ DIO_LO; }
////			CLK_CYCLE;
////		}
////	}
//
//	SPI0_C2 = 0x0B;
//	j = SPI0_D;
//	while((SPI0_S&SPI_S_SPTEF_MASK)==0x00){}
//	for (i = 0; i < count; i++) {
//		SPI0_D = bytes[i];
//		while((SPI0_S&(SPI_S_SPTEF_MASK|SPI_S_SPRF_MASK)) != 0xA0) {}
//		j = SPI0_D;
//	}
//}
//
//inline void write32(uint32_t data)
//{
//	write_bytes((uint8_t *)&data,4);
//}
//
//inline void write8(uint8_t data)
//{
//	write_bytes(&data, 1);
//}
//
//uint32_t read32()
//{
//	int i;
//	volatile uint32_t dat;
//	uint8_t *bytes = (uint8_t *)&dat;
//
//	SPI0_C2 = 0x03;
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//						| PORT_FUNC_SPI;
//	while (SPI0_S&0x80) {bytes[0] = SPI0_D;}
//
//	SPI0_D = 0x00;
//	while((SPI0_S&0x80) == 0x00) {}
//	bytes[0] = SPI0_D;
//
//	SPI0_D = 0x00;
//	while((SPI0_S&0x80) == 0x00) {}
//	bytes[1] = SPI0_D;
//
//	SPI0_D = 0x00;
//	while((SPI0_S&0x80) == 0x00) {}
//	bytes[2] = SPI0_D;
//
//	SPI0_D = 0x00;
//	while((SPI0_S&0x80) == 0x00) {}
//	bytes[3] = SPI0_D;
//
//	return dat;
//}
//
//uint8_t turn_and_response()
//{
//	uint8_t resp = 0;
//
//	DIO_HI;
//	GPIOA_PDDR |= (1 << SWD_IO_PIN);
//	// Set SWD_CLK and SWD_IO to GPIO mode
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//					| PORT_FUNC_GPIO_W_DSE;
//
//
//	// Turn cycle
//	CLK_TRSTN;
//	// Set the DIO pin to input
//	GPIOA_PDDR &= ~(1 << SWD_IO_PIN);
//	CLK_TRSTN;
//
//	__asm("DSB");
//	resp |= DIO_STATE << 2;
//	CLK_CYCLE;
//
//	__asm("DSB");
//	resp |= DIO_STATE << 1;
//	CLK_CYCLE;
//
//	__asm("DSB");
//	resp |= DIO_STATE;
//	CLK_CYCLE;
//
//	return resp;
//}
//
//inline void send_write_parity(uint32_t dat)
//{
//	uint8_t *bytes = (uint8_t *)&dat;
//	// Set SWD_CLK and SWD_IO to GPIO mode
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//					| PORT_FUNC_GPIO_W_DSE;
//
//	bytes[0] ^= bytes[1];
//	bytes[0] ^= bytes[2];
//	bytes[0] ^= bytes[3];
//	bytes[0] = (bytes[0] & 0xF) ^ (bytes[0] >> 4);
//	if (SWDParityTable[bytes[0]]) 	{ DIO_HI; }
//	else 							{ DIO_LO; }
//
//	GPIOA_PDDR |= 1 << SWD_IO_PIN;
//	CLK_CYCLE;
//}
//
//inline int check_read_parity(uint32_t dat)
//{
//	uint8_t *bytes = (uint8_t *)&dat;
//	// Set the DIO pin to input
//	GPIOA_PDDR &= ~(1 << SWD_IO_PIN);
//	// Set SWD_CLK and SWD_IO to GPIO mode
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//					| PORT_FUNC_GPIO_W_DSE;
//
////	CLK_TRSTN;
//	bytes[0] ^= bytes[1];
//	bytes[0] ^= bytes[2];
//	bytes[0] ^= bytes[3];
//	bytes[0] = (bytes[0] & 0xF) ^ (bytes[0] >> 4);
//	bytes[1] = DIO_STATE;
////	CLK_TRSTN;
//	CLK_CYCLE;
//	return (SWDParityTable[bytes[0]]) == bytes[1];
//}
//
//// start = 1
//// parity = even
//// stop = 0
//uint8_t swd_write(int APnDP,uint8_t addr, uint32_t dat)
//{
//	uint8_t cmd;
//	uint8_t count = 0;
//	do {
//		cmd =  (APnDP ? SWD_AP_NDP: 0) | (addr & 0xC) << 1;
//		cmd |= SWD_START | (SWDParityTable[cmd>>1] << 5) | SWD_PARK;
//		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//									| PORT_FUNC_SPI;
//	//						| PORT_FUNC_GPIO_W_DSE;
//		write8(cmd);
//
//		// Get response from target
//		cmd = turn_and_response();
//		CLK_TRSTN;
//
//		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//							| PORT_FUNC_SPI;
//		CLK_TRSTN;
//	} while ((cmd == SWD_RES_WAIT) && (count++ < SWD_MAX_FAIL));
//
//	if (cmd != SWD_RES_ACK) { return cmd; }
//
//	write32(dat);
//
//	send_write_parity(dat);
//	return cmd;
//}
//
//uint8_t swd_read(int APnDP, uint8_t addr, uint32_t *ret)
//{
//	uint8_t cmd;
//	uint8_t count = 0;
//	do {
//		cmd =  (APnDP ? SWD_AP_NDP: 0) | SWD_RNW | (addr & 0xC) << 1;
//		cmd |= SWD_START | (SWDParityTable[cmd>>1] << 5) | SWD_PARK;
//		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//									| PORT_FUNC_SPI;
//	//						| PORT_FUNC_GPIO_W_DSE;
//
//		if (addr&0x40) {
//			dbputstring("cpy: 0x");
////			dbputbyte(cpy);
//			dbputstring(" - addr: 0x");
//			dbputbyte(addr);
//			dbnewline();
//		}
//		write8(cmd);
//
//		cmd = turn_and_response();
//
//		if (cmd != SWD_RES_ACK) {
//			CLK_TRSTN;
//			PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//								| PORT_FUNC_SPI;
//			CLK_TRSTN;
//		}
//	} while ((cmd == SWD_RES_WAIT) && (count++ < SWD_MAX_FAIL));
//
//	if (cmd != SWD_RES_ACK) { return cmd; }
//
//	*ret = read32();
//	if (!check_read_parity(*ret)) { cmd = 0x02; } // parity failed, let's say it was in wait
//
//	PORTA_GPCLR = (1 << (SWD_CLK_PIN+16)) | PORT_FUNC_GPIO_W_DSE;
//	CLK_TRSTN;
//	PORTA_GPCLR = (( (1 << SWD_IO_PIN)) << 16)
//						| PORT_FUNC_SPI;
//	CLK_TRSTN;
////	dbputstring("Addr: 0x");
////	dbputbyte(addr);
////	dbnewline();
////	dbputstring("ret: 0x");
////	dbputword(*ret);
////	dbnewline();
//	return cmd;
//}
//
//void reset_target_swd()
//{
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//						| PORT_FUNC_SPI;
////						| PORT_FUNC_GPIO_W_DSE;
//	write_bytes(BusResetData,7);
//	write_bytes(SWD_change_seq,2);
//	write_bytes(BusResetData,7);
//	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
//						| PORT_FUNC_GPIO_W_DSE;
//
//	DIO_LO;
//	GPIOA_PDDR |= 1 << SWD_IO_PIN;
//	CLK_CYCLE;
//	CLK_CYCLE;
//	CLK_CYCLE;
//}
//
//void clear_faults()
//{
//	uint32_t data;
//	uint8_t ret = swd_read(false, DB_REG_CTRL_STAT, &data);
//	if (ret != 0x04) {return;}
//
//	ret = 0;
//	if (data & C_WDATA_ERR) {
//		ret |= ABT_WD_ERR_CLR;
//	}
//	if (data & C_STICK_ORUN) {
//		ret |= ABT_ORUN_ERR_CLR;
//	}
//	if (data & C_STICKY_CMP) {
//		ret |= ABT_STK_CMP_CLR;
//	}
//	if (data & C_STICKY_ERR) {
//		ret |= ABT_STK_ERR_CLR;
//	}
//	if (ret) {
//		data = ret;
//		swd_write(false, DB_REG_ABORT_ID,data);
//	}
//}
//
//void send_halt()
//{
//	uint32_t data;
//	uint8_t ret;
//	do {
//		ret = swd_write(false,DB_REG_SELECT,0x00);
//		if (ret != SWD_RES_ACK) {
//			clear_faults();
//			continue;
//		}
//		ret = swd_write(true,AP_REG_TAR,(uint32_t)&(CoreDebug->DHCSR));
//		ret = swd_read(true,AP_REG_DRW,&data);
//		do {
//			ret = swd_read(true,AP_REG_DRW,&data);
//		} while (ret != SWD_RES_ACK);
//		data = 0xA05F0003;
//		do {
//			ret = swd_write(true,AP_REG_DRW,data);
//		} while (ret != SWD_RES_ACK);
//
//	} while(0);
//}
//
//void config_clocks()
//{
//	SIM->SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);
//	SIM->CLKDIV1 = 1 << 16;
//	MCG_C1 = 0x02;
//
//	for (volatile uint16_t i = 0; i < 10000; i++) {}
//}
//
//void config_spi()
//{
//	SPI0_BR = 0x01;
//	SPI0_C1 = 0x51; // config for Least Significant Bit First
//	SPI0_C2 = 0x0B;
//	volatile uint8_t j = SPI0_D;
//	j = SPI0_S;
//}
//
//#ifdef _DEBUG
//void config_uart()
//{
//	SIM->SOPT2  |= 1 << 26;
//	SIM->SCGC5 |= 1 << 20;
//	LPUART0_BAUD = 0x0F00001A; // 115200 Baud
//	LPUART0_CTRL = 0x000C0000;
//	PORTB_GPCLR = (((1 << UART_TX_PIN) | (1 << UART_RX_PIN)) << 16)
//					| PORT_FUNC_UART_TX_RX;
//}
//
//void putachar(char c)
//{
//	while ((LPUART0_STAT&LPUART_STAT_TDRE_MASK) == 0) {}
//	LPUART0_DATA = c;
//}
//
//inline void putstring(const char *str)
//{
//	while (*str) { putachar(*str++); }
//}
//
//inline void newline()
//{
////	putachar('\r');
//	putachar('\n');
//}
//
//void putbyte(uint8_t b)
//{
//	if ((b >> 4) < 0xA) { putachar('0'+(b>>4)); }
//	else 				{ putachar('7'+(b>>4)); }
//
//	if ((b&0xF) < 0xA) 	{ putachar('0'+(b&0xF)); }
//	else 				{ putachar('7'+(b&0xF)); }
//}
//
//void putword(uint32_t w)
//{
//	putbyte(((uint8_t *)&w)[3]);
//	putbyte(((uint8_t *)&w)[2]);
//	putbyte(((uint8_t *)&w)[1]);
//	putbyte(((uint8_t *)&w)[0]);
//}
//#endif
//
//int swd_config_mem_access(uint32_t addr)
//{
//	uint8_t ret;
//	ret = swd_write(false,DB_REG_ABORT_ID,0x16);
//	if (ret != SWD_RES_ACK) { return ret; }
//
//	ret = swd_write(false,DB_REG_SELECT,0x00);
//	if (ret != SWD_RES_ACK) { return ret; }
//
//	ret = swd_write(true,AP_REG_TAR,addr);
//	return ret;
//}
//
//int swd_read_mem(uint32_t addr, uint32_t *data)
//{
//	uint8_t ret;
//	ret = swd_config_mem_access(addr);
//	if (ret != SWD_RES_ACK) { return ret; }
//	ret = swd_read(true,AP_REG_DRW,data);
//	if (ret != SWD_RES_ACK) { return ret; }
//	ret = swd_read(false,DB_REG_RD_BUFF,data);
//	return ret;
//}
//
//int swd_write_mem(uint32_t addr, uint32_t data)
//{
//	uint8_t ret;
//	ret = swd_config_mem_access(addr);
//	if (ret != SWD_RES_ACK) { return ret; }
//	ret = swd_write(true,AP_REG_DRW,data);
//	ret = swd_read(true,AP_REG_CSW, &data);
//
//	return ret;
//}
//
//
//int swd_write_reg(uint8_t regNum, uint32_t regData)
//{
//	uint8_t ret;
//	uint32_t tmp;
//	// select the register
//	swd_read_mem((uint32_t)&(CoreDebug->DHCSR), &tmp);
//	swd_write_mem((uint32_t)&(CoreDebug->DCRDR), regData);
//	swd_write_mem((uint32_t)&(CoreDebug->DCRSR), (1<<16) | (regNum & 0x1F));
//	do {
//		ret = swd_read_mem((uint32_t)&(CoreDebug->DHCSR), &regData);
//	} while ((ret == SWD_RES_WAIT) && ((regData & 0x00010000) == 0));
//
//	return ret;
//}
//
//int swd_write_array(uint32_t startAddr, uint8_t *data, uint32_t dataLen)
//{
//	uint8_t ret;
//	uint32_t tmp;
////	ret = swd_config_mem_access(startAddr);
////	if (ret != SWD_RES_ACK) { return ret; }
//	ret = swd_read(true,AP_REG_CSW, &tmp);
//	ret = swd_read(false,DB_REG_RD_BUFF, &tmp);
//	if (ret != SWD_RES_ACK) { return ret; }
//	ret = swd_write(true,AP_REG_CSW, tmp | 0x10);
//	for (uint32_t i = 0; i < dataLen; i+=4) {
//		swd_write_mem(startAddr+(i),data[i+3] | data[i+2]<<8 | data[i+1] << 16 | data[i]<< 24);
//	}
//
//	ret = swd_write(true,AP_REG_CSW, tmp);
//
//	return ret;
//}
//
//void readback(uint32_t startAddr, uint32_t dataLen)
//{
//	uint8_t ret;
//	uint32_t tmp;
//	uint32_t data;
//	ret = swd_config_mem_access(startAddr);
//	if (ret != SWD_RES_ACK) { return; }
//	ret = swd_read(true,AP_REG_CSW, &tmp);
//	if (ret != SWD_RES_ACK) { return; }
//	ret = swd_write(true,AP_REG_CSW, tmp | 0x10);
//	for (uint32_t i = 0; i < dataLen; i+=4) {
//		swd_read_mem(startAddr+(i),&data);
//	}
//
//	ret = swd_write(true,AP_REG_CSW, tmp);
//}

//int enable_debug()
//{
//	uint32_t regData;
//	swd_write(false,DB_REG_SELECT,0x00);
//	swd_write_mem((uint32_t)&(CoreDebug->DHCSR), (DEBUG_MAGIC_KEY << 16) | 3 );
//	swd_read_mem((uint32_t)&(CoreDebug->DHCSR), &regData);
//	swd_write_mem((uint32_t)&(CoreDebug->DEMCR), 0x01000001 );
//}
//
//int reset_and_power_debug()
//{
//	uint32_t data;
//	uint8_t ret;
//	// Reset the bus and confirm the target is configured
//	do {
//		reset_target_swd();
//		ret = swd_read(false, DB_REG_ABORT_ID, &data);
//		ret = swd_write(false, DB_REG_ABORT_ID, 0x16);
//		dbputstring("Data: ");
//		dbputword(data&0xFFF);
//		dbnewline();
//	} while ((ret != 0x04) || ((data & 0xFFF) != 0x477 ));
//
//	// Power up the debug module and confirm that it is up
//	do {
//		ret = swd_write(false,DB_REG_CTRL_STAT, C_DBG_PWR_UP_REQ);
//		if (ret != 0x04) {
//			clear_faults();
//			continue;
//		}
//		ret = swd_read(false,DB_REG_CTRL_STAT,&data);
//	} while ((ret != SWD_RES_ACK) || ((data &  C_DBG_PWR_UP_ACK) != (C_DBG_PWR_UP_ACK)));
//}

//int takeover_and_inject()
//{
//	uint32_t regData = 0;
//	uint8_t count = 0;
//	// switch to MDM-AP
//	enable_debug();
//	swd_write(false,DB_REG_SELECT,0x01000000);
//
//	swd_read(true,MDM_REG_STAT,&regData);
//	swd_read(true,MDM_REG_STAT,&regData);
//	while (((regData & (MDM_STAT_CORE_HALTED)) == 0) && (count++ < SWD_MAX_FAIL)) {
//		swd_write(true,MDM_REG_CTL,MDM_CTL_DBG_REQ|MDM_CTL_CORE_HOLD_RST);
//		swd_read(true,MDM_REG_STAT,&regData);
//		swd_read(true,MDM_REG_STAT,&regData);
//	}
//
//	// switch to MEM-AP
//	swd_write(false,DB_REG_SELECT,0x00000000);
//	for (uint8_t i = 0; i < 13; i++) {
//		regData = i;
//		swd_write_reg(i,regData);
//	}
//
//	regData = ((0x1FFFFE00 + blink_code_len) & 0xFFFFFFFC)+4;
//	swd_write_reg(13,regData); // set the new SP
////	swd_access_reg(true,15,&regData);
//	regData = 0x1FFFFE00;
//	swd_write_reg(14,regData); // link to the start of the injection
//	swd_write_reg(15,regData); // set the new PC to the start of the injection
//	swd_write_array(0x1FFFFE00,blink_code,blink_code_len);
//	readback(0x1FFFFE00,blink_code_len);
//	swd_write(false,DB_REG_SELECT,0x01000000);
//	swd_read(true,MDM_REG_STAT,&regData);
//	swd_read(true,MDM_REG_STAT,&regData);
//	swd_write(false,DB_REG_SELECT,0x00000000);
//
//	reset_and_power_debug();
//
//	for (volatile int i = 0; i < 50000; i++) {}
//
//	enable_debug();
//
//	swd_read(false,DB_REG_CTRL_STAT, &regData);
//
//	swd_write(false,DB_REG_SELECT,0x01000000);
//	swd_read(false,DB_REG_CTRL_STAT, &regData);
//	swd_write(true,MDM_REG_CTL,0x00); // exit the debug state
//
//	return 1;
//}

int main(void)
{
	uint32_t i = 0;
	config_clocks();
    /* Write your code here */
	/* Enable Port B */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC4 |= 1 << 22;
	config_spi();

	PORTA_PCR8 |= PORT_PCR_MUX(3);
	PORTA_PCR9 |= PORT_PCR_MUX(3);
	GPIOA_PDDR |= (1 << SWD_CLK_PIN);// /*(1 << SWD_IO_PIN) |*/ (1 << 12);
	GPIOA_PCOR = 1 << SWD_CLK_PIN; // Make sure the clock is low when shifting to GPIO
#ifdef _DEBUG
	config_uart();
#endif
	/* Enable RED LED as GPIO */
	PORTB_PCR10 |= PORT_PCR_MUX(1);
	/* Set PTB10, PTB11 and PTB13 as outputs */
	GPIOB_PDDR |= 1 << 10;

	/* Turn off LEDs */
	GPIOB_PSOR = 1 << 10;

	uint32_t data = 0x12345678;
	uint8_t ret = 0;
	// Reset the bus and confirm the target is configured
	reset_and_power_debug();

	swd_write(false, DB_REG_SELECT, 0x01000000);
	swd_write(true, MDM_REG_CTL, 0x4);
	swd_read(true, MDM_REG_STAT, &data);
	swd_read(false, DB_REG_RD_BUFF, &data);
	swd_write(false, DB_REG_SELECT, 0x00);
	swd_write(true, AP_REG_CSW, 0x03000042);
	swd_read(true, AP_REG_CSW, &data);
	swd_read(false, DB_REG_RD_BUFF, &data);

	swd_write_mem((uint32_t)&(CoreDebug->DHCSR), 0xA05F0003);

	swd_write_array(SRAM_START,blink_code,blink_code_len);

	swd_write_reg(0xF,SRAM_START);
	swd_write_reg(0xE,SRAM_START+1);

	swd_write_mem((uint32_t)&(CoreDebug->DHCSR), 0xA05F0000);

//	takeover_and_inject();

    while(1){
    	/* Turn on RED LED */
    	GPIOB_PCOR = 1 << 10;
    	for(i = 0; i < delay; i++); /* delay */
    	/* Turn off RED LED */
    	GPIOB_PSOR = 1 << 10;
    	for(i = 0; i < delay; i++); /* delay */

//    	/* Turn on GREEN LED */
//    	GPIOB_PCOR = 1 << 11;
//    	for(i = 0; i < delay; i++); /* delay */
//    	/* Turn off GREEN LED */
//    	GPIOB_PSOR = 1 << 11;
//    	for(i = 0; i < delay; i++); /* delay */
//
//    	/* Turn on BLUE LED */
//    	GPIOB_PCOR = 1 << 13;
//    	for(i = 0; i < delay; i++); /* delay */
//    	/* Turn off BLUE LED */
//    	GPIOB_PSOR = 1 << 13;
//    	for(i = 0; i < delay; i++); /* delay */

////		ret = swd_write(true,AP_REG_DRW,0xFFFF);
////		ret = swd_read(false,DB_REG_CTRL_STAT,&data);
//    	swd_read(false,0x00,&data);
//    	data++;
    	if (!(GPIOB_PDIR & (1 << 5))) {
    		GPIOB_PCOR = 1 << 10 | 1 << 11 | 1 << 12;
        	for(i = 0; i < delay; i++); /* delay */

    		GPIOB_PSOR = 1 << 10 | 1 << 11 | 1 << 12;
        	for(i = 0; i < delay; i++); /* delay */
    	}
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
