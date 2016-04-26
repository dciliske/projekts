#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include "swd.h"
#include "swd_kl03.h"
#include "bsp.h"


#define SWD_MAX_FAIL 50


#define CLK_TRSTN GPIOA_PTOR = (1 << SWD_CLK_PIN)
#define CLK_CYCLE CLK_TRSTN; __asm("DSB"); CLK_TRSTN;
#define DIO_STATE ((GPIOA_PDIR & (1 << SWD_IO_PIN)) >> SWD_IO_PIN)
#define DIO_HI 		GPIOA_PSOR = 1 << SWD_IO_PIN;
#define DIO_LO		GPIOA_PCOR = 1 << SWD_IO_PIN;

#define SWD_AP_NDP 	0x02
#define SWD_RNW 	0x04
#define SWD_START  	0x01
#define SWD_PARK 	0x80

const uint8_t BusResetData[7] = {
		0xFF, 0xFF, 0xFF, 0xFF,
		0xFF, 0xFF, 0xFF
};

const uint8_t SWD_change_seq[2] = {0xE7, 0x9E}; // the magic sequence

const uint8_t SWDParityTable[16] = {
		0x00, 0x01, 0x01, 0x00,
		0x01, 0x00, 0x00, 0x01,
		0x01, 0x00, 0x00, 0x01,
		0x00, 0x01, 0x01, 0x00,
};


static void write_bytes(const uint8_t *bytes, int count)
{
	int i;
	volatile int j;

//	GPIOA_PDDR |= (1 << SWD_IO_PIN);
//	for (i = 0; i < count; i++) {
//		for (j = 0; j < 8; j++) {
//			if ((bytes[0] >> j) & 0x1) 	{ DIO_HI; }
//			else 						{ DIO_LO; }
//			CLK_CYCLE;
//		}
//	}

	SPI0_C2 = 0x0B;
	j = SPI0_D;
	while((SPI0_S&SPI_S_SPTEF_MASK)==0x00){}
	for (i = 0; i < count; i++) {
		SPI0_D = bytes[i];
		while((SPI0_S&(SPI_S_SPTEF_MASK|SPI_S_SPRF_MASK)) != 0xA0) {}
		j = SPI0_D;
	}
}

static inline void write32(uint32_t data)
{
	write_bytes((uint8_t *)&data,4);
}

static inline void write8(uint8_t data)
{
	write_bytes(&data, 1);
}

static uint32_t read32()
{
	volatile uint32_t dat;
	uint8_t *bytes = (uint8_t *)&dat;

	SPI0_C2 = 0x03;
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
						| PORT_FUNC_SPI;
	while (SPI0_S&0x80) {bytes[0] = SPI0_D;}

	SPI0_D = 0x00;
	while((SPI0_S&0x80) == 0x00) {}
	bytes[0] = SPI0_D;

	SPI0_D = 0x00;
	while((SPI0_S&0x80) == 0x00) {}
	bytes[1] = SPI0_D;

	SPI0_D = 0x00;
	while((SPI0_S&0x80) == 0x00) {}
	bytes[2] = SPI0_D;

	SPI0_D = 0x00;
	while((SPI0_S&0x80) == 0x00) {}
	bytes[3] = SPI0_D;

	return dat;
}

static uint8_t turn_and_response()
{
	uint8_t resp = 0;

	DIO_HI;
	GPIOA_PDDR |= (1 << SWD_IO_PIN);
	// Set SWD_CLK and SWD_IO to GPIO mode
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
					| PORT_FUNC_GPIO_W_DSE;


	// Turn cycle
	CLK_TRSTN;
	// Set the DIO pin to input
	GPIOA_PDDR &= ~(1 << SWD_IO_PIN);
	CLK_TRSTN;

	__asm("DSB");
	resp |= DIO_STATE << 2;
	CLK_CYCLE;

	__asm("DSB");
	resp |= DIO_STATE << 1;
	CLK_CYCLE;

	__asm("DSB");
	resp |= DIO_STATE;
	CLK_CYCLE;

	return resp;
}

static inline void send_write_parity(uint32_t dat)
{
	uint8_t *bytes = (uint8_t *)&dat;
	// Set SWD_CLK and SWD_IO to GPIO mode
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
					| PORT_FUNC_GPIO_W_DSE;

	bytes[0] ^= bytes[1];
	bytes[0] ^= bytes[2];
	bytes[0] ^= bytes[3];
	bytes[0] = (bytes[0] & 0xF) ^ (bytes[0] >> 4);
	if (SWDParityTable[bytes[0]]) 	{ DIO_HI; }
	else 							{ DIO_LO; }

	GPIOA_PDDR |= 1 << SWD_IO_PIN;
	CLK_CYCLE;
}

static inline int check_read_parity(uint32_t dat)
{
	uint8_t *bytes = (uint8_t *)&dat;
	// Set the DIO pin to input
	GPIOA_PDDR &= ~(1 << SWD_IO_PIN);
	// Set SWD_CLK and SWD_IO to GPIO mode
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
					| PORT_FUNC_GPIO_W_DSE;

//	CLK_TRSTN;
	bytes[0] ^= bytes[1];
	bytes[0] ^= bytes[2];
	bytes[0] ^= bytes[3];
	bytes[0] = (bytes[0] & 0xF) ^ (bytes[0] >> 4);
	bytes[1] = DIO_STATE;
//	CLK_TRSTN;
	CLK_CYCLE;
	return (SWDParityTable[bytes[0]]) == bytes[1];
}

// start = 1
// parity = even
// stop = 0
uint8_t swd_write(int APnDP,uint8_t addr, uint32_t dat)
{
	uint8_t cmd;
	uint8_t count = 0;
	do {
		cmd =  (APnDP ? SWD_AP_NDP: 0) | (addr & 0xC) << 1;
		cmd |= SWD_START | (SWDParityTable[cmd>>1] << 5) | SWD_PARK;
		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
									| PORT_FUNC_SPI;
	//						| PORT_FUNC_GPIO_W_DSE;
		write8(cmd);

		// Get response from target
		cmd = turn_and_response();
		CLK_TRSTN;

		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
							| PORT_FUNC_SPI;
		CLK_TRSTN;
	} while ((cmd == SWD_RES_WAIT) && (count++ < SWD_MAX_FAIL));

	if (cmd != SWD_RES_ACK) { return cmd; }

	write32(dat);

	send_write_parity(dat);
	return cmd;
}

uint8_t swd_read(int APnDP, uint8_t addr, uint32_t *ret)
{
	uint8_t cmd;
	uint8_t count = 0;
	do {
		cmd =  (APnDP ? SWD_AP_NDP: 0) | SWD_RNW | (addr & 0xC) << 1;
		cmd |= SWD_START | (SWDParityTable[cmd>>1] << 5) | SWD_PARK;
		PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
									| PORT_FUNC_SPI;
	//						| PORT_FUNC_GPIO_W_DSE;

		write8(cmd);

		cmd = turn_and_response();

		if (cmd != SWD_RES_ACK) {
			CLK_TRSTN;
			PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
								| PORT_FUNC_SPI;
			CLK_TRSTN;
		}
	} while ((cmd == SWD_RES_WAIT) && (count++ < SWD_MAX_FAIL));

	if (cmd != SWD_RES_ACK) { return cmd; }

	*ret = read32();
	if (!check_read_parity(*ret)) { cmd = 0x02; } // parity failed, let's say it was in wait

	PORTA_GPCLR = (1 << (SWD_CLK_PIN+16)) | PORT_FUNC_GPIO_W_DSE;
	CLK_TRSTN;
	PORTA_GPCLR = (( (1 << SWD_IO_PIN)) << 16)
						| PORT_FUNC_SPI;
	CLK_TRSTN;
//	dbputstring("Addr: 0x");
//	dbputbyte(addr);
//	dbnewline();
//	dbputstring("ret: 0x");
//	dbputword(*ret);
//	dbnewline();
	return cmd;
}

void reset_target_swd()
{
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
						| PORT_FUNC_SPI;
//						| PORT_FUNC_GPIO_W_DSE;
	write_bytes(BusResetData,7);
	write_bytes(SWD_change_seq,2);
	write_bytes(BusResetData,7);
	PORTA_GPCLR = (((1 << SWD_CLK_PIN) | (1 << SWD_IO_PIN)) << 16)
						| PORT_FUNC_GPIO_W_DSE;

	DIO_LO;
	GPIOA_PDDR |= 1 << SWD_IO_PIN;
	CLK_CYCLE;
	CLK_CYCLE;
	CLK_CYCLE;
}


void clear_faults()
{
	uint32_t data;
	uint8_t ret = swd_read(false, DB_REG_CTRL_STAT, &data);
	if (ret != 0x04) {return;}

	ret = 0;
	if (data & C_WDATA_ERR) {
		ret |= ABT_WD_ERR_CLR;
	}
	if (data & C_STICK_ORUN) {
		ret |= ABT_ORUN_ERR_CLR;
	}
	if (data & C_STICKY_CMP) {
		ret |= ABT_STK_CMP_CLR;
	}
	if (data & C_STICKY_ERR) {
		ret |= ABT_STK_ERR_CLR;
	}
	if (ret) {
		data = ret;
		swd_write(false, DB_REG_ABORT_ID,data);
	}
}

void send_halt()
{
	uint32_t data;
	uint8_t ret;
	do {
		ret = swd_write(false,DB_REG_SELECT,0x00);
		if (ret != SWD_RES_ACK) {
			clear_faults();
			continue;
		}
		ret = swd_write(true,AP_REG_TAR,(uint32_t)&(CoreDebug->DHCSR));
		ret = swd_read(true,AP_REG_DRW,&data);
		do {
			ret = swd_read(true,AP_REG_DRW,&data);
		} while (ret != SWD_RES_ACK);
		data = 0xA05F0003;
		do {
			ret = swd_write(true,AP_REG_DRW,data);
		} while (ret != SWD_RES_ACK);

	} while(0);
}


int swd_config_mem_access(uint32_t addr)
{
	uint8_t ret;
	ret = swd_write(false,DB_REG_ABORT_ID,0x16);
	if (ret != SWD_RES_ACK) { return ret; }

	ret = swd_write(false,DB_REG_SELECT,0x00);
	if (ret != SWD_RES_ACK) { return ret; }

	ret = swd_write(true,AP_REG_TAR,addr);
	return ret;
}

int swd_read_mem(uint32_t addr, uint32_t *data)
{
	uint8_t ret;
	ret = swd_config_mem_access(addr);
	if (ret != SWD_RES_ACK) { return ret; }
	ret = swd_read(true,AP_REG_DRW,data);
	if (ret != SWD_RES_ACK) { return ret; }
	ret = swd_read(false,DB_REG_RD_BUFF,data);
	return ret;
}

int swd_write_mem(uint32_t addr, uint32_t data)
{
	uint8_t ret;
	ret = swd_config_mem_access(addr);
	if (ret != SWD_RES_ACK) { return ret; }
	ret = swd_write(true,AP_REG_DRW,data);
	ret = swd_read(true,AP_REG_CSW, &data);

	return ret;
}


int swd_write_reg(uint8_t regNum, uint32_t regData)
{
	uint8_t ret;
	uint32_t tmp;
	// select the register
	swd_read_mem((uint32_t)&(CoreDebug->DHCSR), &tmp);
	swd_write_mem((uint32_t)&(CoreDebug->DCRDR), regData);
	swd_write_mem((uint32_t)&(CoreDebug->DCRSR), (1<<16) | (regNum & 0x1F));
	do {
		ret = swd_read_mem((uint32_t)&(CoreDebug->DHCSR), &regData);
	} while ((ret == SWD_RES_WAIT) && ((regData & 0x00010000) == 0));

	return ret;
}

int swd_read_reg(uint8_t regNum, uint32_t *regData)
{
	uint8_t ret;
	swd_write_mem((uint32_t)&(CoreDebug->DCRSR), (regNum & 0x1F));
	do {
		ret = swd_read_mem((uint32_t)&(CoreDebug->DHCSR), regData);
	} while ((ret == SWD_RES_WAIT) && ((*regData & 0x00010000) == 0));
	if (ret == SWD_RES_ACK) {
		do {
			ret = swd_read_mem((uint32_t)&(CoreDebug->DCRDR), regData);
		} while (ret == SWD_RES_WAIT);
	}

	return ret;
}

int swd_write_array(uint32_t startAddr, uint8_t *data, uint32_t dataLen)
{
	uint8_t ret;
	uint32_t tmp;
	ret = swd_config_mem_access(startAddr);
	if (ret != SWD_RES_ACK) { return ret; }
	ret = swd_read(true,AP_REG_CSW, &tmp);
	ret = swd_read(false,DB_REG_RD_BUFF, &tmp);
	if (ret != SWD_RES_ACK) { return ret; }
	ret = swd_write(true,AP_REG_CSW, tmp | 0x10);
	for (uint32_t i = 0; i < dataLen; i+=4) {
		swd_write(true,AP_REG_DRW,data[i] | data[i+1]<<8 | data[i+2] << 16 | data[i+3]<< 24);
	}

	ret = swd_write(true,AP_REG_CSW, tmp);

	return ret;
}

void readback(uint32_t startAddr, uint32_t dataLen)
{
	uint8_t ret;
	uint32_t tmp;
	uint32_t data;
	ret = swd_config_mem_access(startAddr);
	if (ret != SWD_RES_ACK) { return; }
	ret = swd_read(true,AP_REG_CSW, &tmp);
	if (ret != SWD_RES_ACK) { return; }
	ret = swd_write(true,AP_REG_CSW, tmp | 0x10);
	for (uint32_t i = 0; i < dataLen; i+=4) {
		swd_read_mem(startAddr+(i),&data);
	}

	ret = swd_write(true,AP_REG_CSW, tmp);
}

int reset_and_power_debug()
{
	uint32_t data;
	uint8_t ret;
	// Reset the bus and confirm the target is configured
	do {
		reset_target_swd();
		ret = swd_read(false, DB_REG_ABORT_ID, &data);
		ret = swd_write(false, DB_REG_ABORT_ID, 0x16);
		dbputstring("Data: ");
		dbputword(data&0xFFF);
		dbnewline();
	} while ((ret != 0x04) || ((data & 0xFFF) != 0x477 ));

	// Power up the debug module and confirm that it is up
	do {
		ret = swd_write(false,DB_REG_CTRL_STAT, C_DBG_PWR_UP_REQ);
		if (ret != 0x04) {
			clear_faults();
			continue;
		}
		ret = swd_read(false,DB_REG_CTRL_STAT,&data);
	} while ((ret != SWD_RES_ACK) || ((data &  C_DBG_PWR_UP_ACK) != (C_DBG_PWR_UP_ACK)));
}
