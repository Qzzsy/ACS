/**
 ******************************************************************************
 * @file        sram.c
 * @author      �Ž�����С��
 * @version     V1.0.0
 * @date        2017-11-04
 * @brief       �ļ��ڰ���SRAM��FSMC���������ã�����FSMC���������ź͵�ַ�����Լ�SRAM
                �Ŀ������ţ�������SRAM�Ŀ���ʱ��
 * @note        ʹ��NOR/SRAM�� Bank1.sector3,��ַλHADDR[27,26]=10 
                ��IS61LV51216/IS62WV51216,��ַ�߷�ΧΪA0~A18
 * @History
 * Date           Author    version    		Notes
 * 2017-11-04     ZSY       V1.0.0      first version.
 */
	
/* Includes ------------------------------------------------------------------*/
#include "sram.h"	      

/* global variable Declaration -----------------------------------------------*/


/* User function Declaration --------------------------------------------------*/



/* User functions -------------------------------------------------------------*/

/**
 * @func    fsmc_sram_config
 * @brief   sram��FSMCӲ�����ã�����sram��FSMC�����ߺ͵�ַ�ߣ�ʱ��Ϳ������Լ�FSMC
            ���������
 * @note    ��Ҫ������Ӧ��ʱ�Ӻ͸���
 * @retval  ��
 */  						   
void fsmc_sram_config(void)
{ 	
    /* Define the structure variables that need to be used */
    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	
    /* config SRAM gpio clock base on FSMC */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | 
                           RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
	
    /* Enable the FSMC Clock */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);	
    
    /**	config PIN Status	
     * alternate function IO output
     * push-Pull output
     * speed is 100MHz
     * pull output
     */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				
    
    /**	config SRAM data lines base on FSMC
     * data line ��FSMC-D0-D15��PD 14 15 0 1 PE 7 8 9 10 11 12 13 14 15��PD 8,9,10
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;	
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/* Initialize load configuration */				
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;	
    GPIO_Init(GPIOE, &GPIO_InitStructure);					/* Initialize load configuration */ 
	
	/**	config SRAM address lines base on FSMC
	 * address line ��FSMC-A0-A18��PD 11 12 13, PF 0 1 2 3 4 5 12 13 14 15��PG 0 1 2 4 5
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/* Initialize load configuration */	 
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 |
                                                                GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);					/* Initialize load configuration */	  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | 
                                GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOG, &GPIO_InitStructure);					/* Initialize load configuration */	 
    
    /** config SRAM control lines base on FSMC
     * PD4-FSMC_NOE  :SRAM-OE
     * PD5-FSMC_NWE  :SRAM-WE
	 * PG10-FSMC_NE3  :SRAM-CE
     * PE0-FSMC_NBL0 :SRAM-SRAM_UB
     * PE1-FSMC_NBL1 :SRAM-SRAM_LB
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;					
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/* Initialize load configuration */ 
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;					
    GPIO_Init(GPIOG, &GPIO_InitStructure);					/* Initialize load configuration */ 
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;					
    GPIO_Init(GPIOE, &GPIO_InitStructure);					/* Initialize load configuration */ 

    /** config SRAM FSMC AF line(data line)
     * alternate function IO PD0��PD1��PD8��PD9��PD10��PD14��PD15 to FSMC output
     * alternate function IO PE7��PE8��PE9��PE10��PE11��PE12��PE13��PE14��PE15 to FSMC output
     */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		//PD0, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		//PD1, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC); 	//PD8, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		//PD9, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);	//PD10, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);	//PD14, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);	//PD15, AF12
    
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		//PE7, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		//PE8, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		//PE9, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);	//PE10, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);	//PE11, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);	//PE12, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);	//PE13, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);	//PE14, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);	//PE15, AF12
    
    /** config SRAM FSMC AF line��address line��
     * alternate function IO PD11��PD12��PD13 to FSMC output
     * alternate function IO PF0��PF1��PF2��PF3��PF4��PF5��PF12��PF13��PF14��PF15 to FSMC output
     * alternate function IO PG0��PG1��PG2��PG3��PG4��PG5 to FSMC output
     */ 
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);	//PD11, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);	//PD12, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);	//PD13, AF12
                            
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0, GPIO_AF_FSMC);		//PF0, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF_FSMC);		//PF1, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2, GPIO_AF_FSMC);		//PF2, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource3, GPIO_AF_FSMC);		//PF3, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF_FSMC);		//PF4, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5, GPIO_AF_FSMC);		//PF5, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);	//PF12, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource13, GPIO_AF_FSMC);	//PF13, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource14, GPIO_AF_FSMC);	//PF14, AF12
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource15, GPIO_AF_FSMC);	//PF15, AF12
    
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource0, GPIO_AF_FSMC);		//PG0, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource1, GPIO_AF_FSMC);		//PG1, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource2, GPIO_AF_FSMC);		//PG2, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource3, GPIO_AF_FSMC);		//PG3, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource4, GPIO_AF_FSMC);		//PG4, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource5, GPIO_AF_FSMC);		//PG5, AF12
    
    /** config SRAM FSMC AF line��control line��
     * alternate function IO PD4, PD5, PE0, PE1, PG10 to FSMC output
     */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		//PD4, AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC); 	//PD5, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_FSMC);		//PE0, AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_FSMC);		//PE1, AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);	//PG10, AF12
    
    readWriteTiming.FSMC_AddressSetupTime = 0x02;       //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime  = 0x00;       //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
    readWriteTiming.FSMC_DataSetupTime    = 0x0A;       //���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 6*9=54ns	 
    
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode  = FSMC_AccessMode_A;   //ģʽA 
    
    /**	Configuring FSMC Write Timings
     * Address Setup time is 9 HCLK = 54ns          ��ַ����ʱ��
     * Address Hold time                            ��ַ����ʱ��
     * Data Setup time is 6ns*9 a hclk = 54ns       ���ݱ���ʱ��
     */
    writeTiming.FSMC_AddressSetupTime = 8;	      					
    writeTiming.FSMC_AddressHoldTime  = 0x00;	 							
    writeTiming.FSMC_DataSetupTime    = 8;		 		
    
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;					
    writeTiming.FSMC_CLKDivision = 0x00;									
    writeTiming.FSMC_DataLatency = 0x00;										
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;        /* mode A */
    
    /* config SRAM control base for FSMC */
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM3;                 //��������ʹ��NE3��//Ҳ�Ͷ�ӦBTCR[6],[7]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable; 		//���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_SRAM;                //FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;            //�洢�����ݿ��Ϊ16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;        //FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;			//�洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable;            //��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;                    //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &writeTiming;                        //дʱ��
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                                               //��ʼ��FSMC����
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);  																	
}  
	
/*	���´���Ϊ������ */
////��ָ����ַ(WriteAddr+Bank1_SRAM3_ADDR)��ʼ,����д��n���ֽ�.
////pBuffer:�ֽ�ָ��
////WriteAddr:Ҫд��ĵ�ַ
////n:Ҫд����ֽ���
//void FSMC_SRAM_WriteBuffer(u8* pBuffer,u32 WriteAddr,u32 n)
//{
//	for(;n!=0;n--)  
//	{										    
//		*(vu8*)(Bank1_SRAM3_ADDR+WriteAddr)=*pBuffer;	  
//		WriteAddr++;
//		pBuffer++;
//	}   
//}																			    
////��ָ����ַ((WriteAddr+Bank1_SRAM3_ADDR))��ʼ,��������n���ֽ�.
////pBuffer:�ֽ�ָ��
////ReadAddr:Ҫ��������ʼ��ַ
////n:Ҫд����ֽ���
//void FSMC_SRAM_ReadBuffer(u8* pBuffer,u32 ReadAddr,u32 n)
//{
//	for(;n!=0;n--)  
//	{											    
//		*pBuffer++=*(vu8*)(Bank1_SRAM3_ADDR+ReadAddr);    
//		ReadAddr++;
//	}  
//} 
// 
uint32_t EXT_SRAM_SIZE = 0x1000;
///*
//*********************************************************************************************************
//*	�� �� ��: bsp_TestExtSRAM
//*	����˵��: ɨ������ⲿSRAM
//*	��    ��: ��
//*	�� �� ֵ: 0 ��ʾ����ͨ���� ����0��ʾ����Ԫ�ĸ�����
//*********************************************************************************************************
//*/
uint32_t data[2048] __attribute__((at(0x68000000))) = {0};
uint8_t bsp_TestExtSRAM(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* дSRAM */
	pSRAM = (uint32_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* ��SRAM */
	err = 0;
	pSRAM = (uint32_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}
	if (err >  0)
	{
		return  (4 * err);
	}

	/* ��SRAM �������󷴲�д�� */
	pSRAM = (uint32_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* �ٴαȽ�SRAM������ */
	err = 0;
	pSRAM = (uint32_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < EXT_SRAM_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}

	/* ���԰��ֽڷ�ʽ����, Ŀ������֤ FSMC_NBL0 �� FSMC_NBL1 ���� */
	pBytes = (uint8_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* �Ƚ�SRAM������ */
	err = 0;
	pBytes = (uint8_t *)Bank1_SRAM3_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}







