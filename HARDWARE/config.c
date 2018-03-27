#include "config.h"
#include "stm32f4xx.h"
#include "stm32f4xx_flash.h"
#include "msg_code_hardware.h"

SYS_CONFIG s_cfg;

//FLASH address
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//sector 0, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//sector 1, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//sector 2, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//sector 3, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//sector 4, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//sector 5, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//sector 6, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//sector 7, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//sector 8, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//sector 9, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//sector 10,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//sector 11,128 Kbytes

uint16_t STMFLASH_GetFlashSector(u32 addr) {
	if(addr<ADDR_FLASH_SECTOR_1) {
		return FLASH_Sector_0;
	}
	else if(addr<ADDR_FLASH_SECTOR_2) {
		return FLASH_Sector_1;
	}
	else if(addr<ADDR_FLASH_SECTOR_3) {
		return FLASH_Sector_2;
	}
	else if(addr<ADDR_FLASH_SECTOR_4) {
		return FLASH_Sector_3;
	}
	else if(addr<ADDR_FLASH_SECTOR_5) {
		return FLASH_Sector_4;
	}
	else if(addr<ADDR_FLASH_SECTOR_6) {
		return FLASH_Sector_5;
	}
	else if(addr<ADDR_FLASH_SECTOR_7) {
		return FLASH_Sector_6;
	}
	else if(addr<ADDR_FLASH_SECTOR_8) {
		return FLASH_Sector_7;
	}
	else if(addr<ADDR_FLASH_SECTOR_9) {
		return FLASH_Sector_8;
	}
	else if(addr<ADDR_FLASH_SECTOR_10) {
		return FLASH_Sector_9;
	}
	else if(addr<ADDR_FLASH_SECTOR_11) {
		return FLASH_Sector_10;
	}
	return FLASH_Sector_11;	
}

u8 readByteFlash(u32 faddr) {
	return *(vu8*)faddr; 
}

void writeFlash(u32 faddr,u8 *pBuffer,u32 num) {
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(faddr<ADDR_FLASH_SECTOR_0) {
		return;	//chaeck addr
	}
	FLASH_Unlock();									//unlock flash
  FLASH_DataCacheCmd(DISABLE);//disable cache
 		
	addrx=faddr;				//start addr
	endaddr=faddr+num;	//end addr
	
	//consider erase
	if(addrx<0X1FFF0000) {
		while(addrx<endaddr) {
			if(readByteFlash(addrx)!=0XFF) {   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V
				if(status!=FLASH_COMPLETE) {
					break;	//error
				}
			}
			else {
				addrx++;
			}
		} 
	}
	//write
	if(status==FLASH_COMPLETE) {
		while(faddr<endaddr) {
			if(FLASH_ProgramByte(faddr,*pBuffer)!=FLASH_COMPLETE) {
				break;	//error
			}
			faddr++;
			pBuffer++;
		} 
	}
  FLASH_DataCacheCmd(ENABLE);	//enable cache
	FLASH_Lock();//lock flash
}

/** initialize external interrupt */
void initExti_config(void) {
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;

	//initialize GPIO
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//enable GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//INPUT
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//pull-up
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//enable SYSCFG
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);//PA8 --> exti8

	/* configure EXTI8 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;//LINE8
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//interrupt
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //falling to trigger
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//enable LINE8
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//EXTI 8
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//priority 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//sub priority 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//enable
	NVIC_Init(&NVIC_InitStructure);
}

/** reset config to default */
void defaultConfig(void) {
	u8 i;
	u8 *pCfg = (u8*)&s_cfg;
	//default value
	s_cfg.header[0] = 0xaa;
	s_cfg.header[1] = 0xaa;
	//local IP
	s_cfg.ip_addr[0] = 10;
	s_cfg.ip_addr[1] = 100;
	s_cfg.ip_addr[2] = 0;
	s_cfg.ip_addr[3] = 127;
	//local mask
	s_cfg.ip_mask[0] = 255;
	s_cfg.ip_mask[1] = 255;
	s_cfg.ip_mask[2] = 255;
	s_cfg.ip_mask[3] = 0;
	//gate IP
	s_cfg.ip_gate[0] = 10;
	s_cfg.ip_gate[1] = 100;
	s_cfg.ip_gate[2] = 0;
	s_cfg.ip_gate[3] = 1;
	//listen port
	s_cfg.listen_port = 4000;
	//remote IP
	s_cfg.remote_ip[0] = 10;
	s_cfg.remote_ip[1] = 100;
	s_cfg.remote_ip[2] = 0;
	s_cfg.remote_ip[3] = 104;
	//remote port
	s_cfg.remote_port = 3000;
	//baud rate and parity
	for(i=0;i<UART_FSMC_NUM;i++) {
		s_cfg.baud_rates[i] = 115200;
		s_cfg.parities[i]   = 0;
	}
	//query cycle
	s_cfg.query_cycle_s = 1;
	//write to flash
	writeFlash(ADDR_FLASH_SECTOR_11,pCfg,sizeof(SYS_CONFIG));
}

/** prepare system configuration */
void prepareConfig(void) {
	u8 i,h0,h1;
	u8 *pCfg = (u8*)&s_cfg;
	
	//read header
	h0 = readByteFlash(ADDR_FLASH_SECTOR_11);
	h1 = readByteFlash(ADDR_FLASH_SECTOR_11+1);
	if((h0==0xaa) && (h1==0xaa)) {
		//existing configurature
		for(i=0;i<sizeof(SYS_CONFIG);i++) {
			pCfg[i] = readByteFlash(ADDR_FLASH_SECTOR_11+i);
		}
	}
	else {
		defaultConfig();
	}

	//prepare exti
	initExti_config();
}

/** get system configuration */
SYS_CONFIG * getConfig(void) {
	return &s_cfg;
}

/** set system configuration */
void setConfig(SYS_CONFIG n_cfg) {
	//copy config
	s_cfg = n_cfg;
	//make sure header is right
	s_cfg.header[0] = 0xaa;
	s_cfg.header[1] = 0xaa;
	//write to flash
	writeFlash(ADDR_FLASH_SECTOR_11,(u8*)&s_cfg,sizeof(SYS_CONFIG));
}

/** apply configuration by reboot */
void applyConfig(void) {
	//reboot signal
	publishMsg(MSG_CONFIG_APPLY,0);
}

//EXTI8 handler
void EXTI9_5_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line8)==SET) {
		defaultConfig();
		applyConfig();
	}
	EXTI_ClearITPendingBit(EXTI_Line9|EXTI_Line8|EXTI_Line7|EXTI_Line6|EXTI_Line5);
}
