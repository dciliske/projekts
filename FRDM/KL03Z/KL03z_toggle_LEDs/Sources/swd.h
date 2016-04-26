#ifndef __SWD_H
#define __SWD_H

#define DB_REG_ABORT_ID 	(0x00)
#define DB_REG_CTRL_STAT 	(0x04)
#define DB_REG_SELECT 		(0x08)
#define DB_REG_RD_BUFF 		(0x0C)

#define AP_REG_CSW 			(0x00)
#define AP_REG_TAR 			(0x04)
#define AP_REG_DRW 			(0x0C)

#define ABT_ORUN_ERR_CLR 	(1 << 4)
#define ABT_WD_ERR_CLR 		(1 << 3)
#define ABT_STK_ERR_CLR 	(1 << 2)
#define ABT_STK_CMP_CLR 	(1 << 1)
#define ABT_DAP_ABT 		(1 << 0)

#define C_SYS_PWR_UP_ACK 	(1 << 31)
#define C_SYS_PWR_UP_REQ 	(1 << 30)
#define C_DBG_PWR_UP_ACK 	(1 << 29)
#define C_DBG_PWR_UP_REQ 	(1 << 28)
#define C_DBG_RST_ACK 		(1 << 27)
#define C_DBG_RST_REQ 		(1 << 26)
#define C_TRNCNT_MASK 		(0xFFF << 12)
#define C_MASKLANE_MASK 	(0xF << 8)
#define C_WDATA_ERR 		(1 << 7)
#define C_READ_OK 			(1 << 6)
#define C_STICKY_ERR 		(1 << 5)
#define C_STICKY_CMP 		(1 << 4)
#define C_TRN_MODE_MASK 	(0x3 << 2)
#define C_STICK_ORUN 		(1 << 1)
#define C_ORUN_DETECT 		(1 << 0)

#define SWD_RES_ACK 		(0x04)
#define SWD_RES_WAIT 		(0x02)
#define SWD_RES_FAULT 		(0x01)

typedef struct
{
  __IO uint32_t DHCSR;                   /*!< Offset: 0x000 (R/W)  Debug Halting Control and Status Register    */
  __O  uint32_t DCRSR;                   /*!< Offset: 0x004 ( /W)  Debug Core Register Selector Register        */
  __IO uint32_t DCRDR;                   /*!< Offset: 0x008 (R/W)  Debug Core Register Data Register            */
  __IO uint32_t DEMCR;                   /*!< Offset: 0x00C (R/W)  Debug Exception and Monitor Control Register */
} CoreDebug_Type;

#define DEBUG_MAGIC_KEY 	(0xA05F)
#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address            */
#define CoreDebug           ((CoreDebug_Type *)     CoreDebug_BASE)   /*!< Core Debug configuration struct    */

uint8_t swd_write(int APnDP,uint8_t addr, uint32_t dat);
uint8_t swd_read(int APnDP, uint8_t addr, uint32_t *ret);
void reset_target_swd();
void clear_faults();
void send_halt();
int swd_config_mem_access(uint32_t addr);
int swd_read_mem(uint32_t addr, uint32_t *data);
int swd_write_mem(uint32_t addr, uint32_t data);
int swd_write_reg(uint8_t regNum, uint32_t regData);
int swd_read_reg(uint8_t regNum, uint32_t *regData);
int swd_write_array(uint32_t startAddr, uint8_t *data, uint32_t dataLen);
void readback(uint32_t startAddr, uint32_t dataLen);
int reset_and_power_debug();

#endif /* #ifndef __SWD_H */
