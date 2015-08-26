#ifndef __SWD_KL03
#define __SWD_KL03

#define MDM_REG_STAT 			0x00
#define MDM_REG_CTL 			0x04

#define MDM_CTL_VLLS_STAT_ACK	(1<<7)
#define MDM_CTL_VLLS_DBG_ACK 	(1<<6)
#define MDM_CTL_VLLS_DBG_REQ 	(1<<5)
#define MDM_CTL_CORE_HOLD_RST 	(1<<4)
#define MDM_CTL_SYS_RST_REQ 	(1<<3)
#define MDM_CTL_DBG_REQ 		(1<<2)
#define MDM_CTL_DBG_DISABLE 	(1<<1)
#define MDM_CTL_FLASH_ME_IP 	(1<<0)

#define MDM_STAT_CORE_SLEEP 	(1<<18)
#define MDM_STAT_CORE_SLEEP_DEEP (1<<17)
#define MDM_STAT_CORE_HALTED 	(1<<16)
#define MDM_STAT_VLLS_MODE_EXIT (1<<10)
#define MDM_STAT_VLP_MODE 		(1<<8)
#define MDM_STAT_LP_EN 			(1<<7)
#define MDM_STAT_BDOOR_KEY_EN 	(1<<6)
#define MDM_STAT_FLASH_ME_EN 	(1<<5)
#define MDM_STAT_SYS_RST 		(1<<3)
#define MDM_STAT_SYS_SEC 		(1<<2)
#define MDM_STAT_FLASH_RDY 		(1<<1)
#define MDM_STAT_FLASH_ME_ACK 	(1<<0)


#endif /* #ifndef __SWD_KL03 */
