/**
 ******************************************************************************
 * @file      lcd_fsmc.c
 * @author    �Ž�����С��
 * @version   V1.0.3
 * @date      2018-01-26
 * @brief     ����ļ���Һ����ʼ���ļ����ڴ��ļ��ڽ���Һ�������û���һЩ�����Ĳ���
              Һ��������оƬΪR61529����оƬ
 * @History
 * Date           Author    version    		Notes
 * 2017-10-31       ZSY     V1.0.0      first version.
 * 2017-11-27       ZSY     V1.0.1      ����LCD�Ĳ�������������LCD����
 * 2018-01-16       ZSY     V1.0.2      �Ű��ʽ������ǿ���ӻ�
 * 2018-01-26       ZSY     V1.0.3      �����ֶ��壬���˽�к͹��к궨��
 */
	
/* Includes ------------------------------------------------------------------*/
#include "lcd_fsmc.h"
#include "rtthread.h"
#include "pwm.h"

/* Private macro Definition --------------------------------------------------*/



/* ����LCD��λ�����ĺ궨�� */  
#define LCD_GPIO_RESET          GPIO_Pin_6
#define LCD_RESET_PORT          GPIOD
#define LCD_RESET_RCC           RCC_AHB1Periph_GPIOD
#define LCD_RESET_WRITE_H GPIO_SetBits(LCD_RESET_PORT, LCD_GPIO_RESET)
#define LCD_RESET_WRITE_L GPIO_ResetBits(LCD_RESET_PORT, LCD_GPIO_RESET)
/* End private macro Definition ----------------------------------------------*/

/* global variable Declaration -----------------------------------------------*/

lcd_dev_typedef lcd_dev;
lcd_func_typedef lcd;

/* User function Declaration --------------------------------------------------*/
static uint32_t lcd_read_id(void);

void lcd_set_disp_dir(uint8_t dir);
void lcd_clr_scr(uint16_t p_color);

/* User functions -------------------------------------------------------------*/

/**
 * @func    lcd_config
 * @brief   Һ��FSMCӲ�����ã�����Һ����FSMC�����ߣ�ʱ��Ϳ������Լ�FSMC����
            ������
 * @note    ��Ҫ������Ӧ��ʱ�Ӻ͸���
 * @retval  ��
 */
static void lcd_config(void)
{ 	
    /*	Define the structure variables that need to be used	*/
    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;
    
    /* config lcd gpio clock base on FSMC */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);
    
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
    
    /**	config tft data lines base on FSMC
     * data line ��FSMC-D0-D15��PD 14 15 0 1 PE 7 8 9 10 11 12 13 14 15��PD 8,9,10
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;	
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/*	Initialize load configuration	*/				
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;	
    GPIO_Init(GPIOE, &GPIO_InitStructure);					/*	Initialize load configuration */ 
    
    /** config tft control lines base on FSMC
     * PD4-FSMC_NOE  :LCD-RD
     * PD5-FSMC_NWE  :LCD-WR
     * PG12-FSMC_NE4  :LCD-CS
     * PE6-FSMC_A22 :LCD-DC
     */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;					
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/*	Initialize load configuration */ 
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;					
    GPIO_Init(GPIOG, &GPIO_InitStructure);	
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					
    GPIO_Init(GPIOE, &GPIO_InitStructure);	
    
    /** config tft reset line
     * LCD-RESET line: PD13
     * Standard output mode
     * push-Pull output
     * speed is 100MHz
     * pull output
     */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;					
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				
    GPIO_Init(GPIOD, &GPIO_InitStructure);					/*	Initialize load configuration */ 
    
    /** config tft FSMC AF line
     * alternate function IO PD0��PD1��PD8��PD9��PD10��PD14��PD15 to FSMC output
     */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		//PD0,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		//PD1,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC); 	//PD8,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		//PD9,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);	//PD10,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);	//PD14,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);	//PD15,AF12
    
    /** config tft FSMC AF line
     * alternate function IO PE7��PE8��PE9��PE10��PE11��PE12��PE13��PE14��PE15 to FSMC output
     */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);		//PE7,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);		//PE8,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);		//PE9,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);	//PE10,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);	//PE11,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);	//PE12,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);	//PE13,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);	//PE14,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);	//PE15,AF12
    
    /** config tft FSMC AF line
     * alternate function IO PD4, PD5, PD7, PD11 to FSMC output
     */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);		//PF4,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC); 	//PF5,AF12
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_FSMC);     //PF11,AF12
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);    //PF7,AF12
    
    readWriteTiming.FSMC_AddressSetupTime = 0x02;       //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime  = 0x00;       //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�	
    readWriteTiming.FSMC_DataSetupTime    = 0x0A;       //���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 6*9=54ns	 
    
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x01;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode  = FSMC_AccessMode_A;	 //ģʽA 
    
    /**	Configuring FSMC Write Timings
     * Address Setup time is 9 HCLK = 54ns 			��ַ����ʱ��
     * Address Hold time												��ַ����ʱ��
     * Data Setup time is 6ns*9 a hclk = 54ns		���ݱ���ʱ��
     */
    writeTiming.FSMC_AddressSetupTime = 8;	      					
    writeTiming.FSMC_AddressHoldTime  = 0x00;	 							
    writeTiming.FSMC_DataSetupTime    = 8;		 								
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;					
    writeTiming.FSMC_CLKDivision = 0x01;									
    writeTiming.FSMC_DataLatency = 0x00;										
    writeTiming.FSMC_AccessMode  = FSMC_AccessMode_A;	 			/* mode B */
    
    /* config tft control base for FSMC	*/
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM4;                 //��������ʹ��NE1��Ҳ�Ͷ�ӦBTCR[6],[7]��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;         //���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_SRAM;                //FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;            //�洢�����ݿ��Ϊ16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;        //FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;          //�洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable;            //��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;                    //��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &writeTiming;                        //дʱ��
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                                               //��ʼ��FSMC����
    
    /* Enable FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  		
    
    
    RCC_AHB1PeriphClockCmd(LCD_LIGHT_RCC | LCD_RESET_RCC, ENABLE);
    /**	config PIN Status	
     * alternate function IO output
     * push-Pull output
     * speed is 100MHz
     * pull output
     */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
    
//    GPIO_InitStructure.GPIO_Pin = LCD_GPIO_LIGHT;					
//    GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);					/* Initialize load configuration */ 
    
    
    GPIO_InitStructure.GPIO_Pin = LCD_GPIO_RESET;					
    GPIO_Init(LCD_RESET_PORT, &GPIO_InitStructure);					/* Initialize load configuration */ 
    
    GPIO_SetBits(LCD_RESET_PORT, LCD_GPIO_RESET);
}  

/**
 * @func    lcd_write_comm
 * @brief   lcdд����Ĵ���
 * @param   regval ָ���ļĴ�����ַ
 * @retval  ��
 */
static void lcd_write_comm(__IO uint16_t  regval)
{   
    LCD_OPERATION->REG = regval;			/* д��Ҫд�ļĴ������ */
}

/**
 * @func    lcd_write_comm
 * @brief   lcdд����
 * @param   data Ҫд�������
 * @retval  ��
 */
static void lcd_write_data(__IO uint16_t  data)
{	  
    LCD_OPERATION->RAM = data;		 
}		

/**
 * @func    lcd_wr_reg
 * @brief   ��ָ���ļĴ���д����
 * @param   index �Ĵ���
 * @param   congfig_temp ����
 * @retval  ��
 */
static void lcd_wr_reg(uint16_t index, uint16_t congfig_temp)
{
    LCD_OPERATION->REG = index;
    LCD_OPERATION->REG = congfig_temp;
}

/**
 * @func    lcd_read_data
 * @brief   lcd������
 * @retval  ����������
 */
static uint16_t lcd_read_data(void)
{
    vu16 ram;			//��ֹ���Ż�
    ram = LCD_OPERATION->RAM;	
    return ram;	 
}	

/**
 * @func    lcd_read_reg
 * @brief   lcd���Ĵ���
 * @param   reg:�Ĵ�����ַ
 * @retval  ����������
 */
static uint16_t lcd_read_reg(uint16_t reg)
{										   
    lcd_write_comm(reg);		//д��Ҫ���ļĴ������
    return LCD_OPERATION->RAM;		//���ض�����ֵ
}  

/**
 * @func    lcd_wr_ram_prepare
 * @brief   lcd��ʼд�����
 * @note    һ���ڲ�������ʱ����
 * @retval  ��
 */
static void lcd_wr_ram_prepare(void)
{
    LCD_OPERATION->REG = lcd_dev.wramcmd;
}

/**
 * @func    lcd_delay
 * @brief   lcd������ʱ�����ڳ�ʼ������
 * @param   n_count ��ʱ�Ĵ�С
 * @retval  ��
 */
static void lcd_delay(__IO uint32_t n_count)
{	
    volatile int i;
    for (i=0; i < 0XFFFF; i++)
        for (; n_count != 0; n_count--);
    return ;
}  

/**
 * @func    lcd_reset
 * @brief   lcd��λ���������ڸ�λlcd�����мĴ���
 * @note    Ҫ��ʱ�㹻��ʱ�������lcd��λ
 * @retval  ��
 */
static void lcd_reset(void)
{		
    /* write 0 to lcd reset line */
    LCD_RESET_WRITE_L;
    lcd_delay(10000);
    lcd_delay(10000);
    lcd_delay(10000);
    lcd_delay(10000);
    
    /* write 1 to lcd reset line */	
    LCD_RESET_WRITE_H;		 	 
    lcd_delay(10000);	
    lcd_delay(10000);
    lcd_delay(10000);
    lcd_delay(10000);
} 

/**
 * @func    lcd_config_loal
 * @brief   ���ݲ�ͬ��LCD���ز�ͬ�Ĳ���
 * @retval  ��
 */
static void lcd_config_loal(void)
{
    if (lcd_dev.id == 0x61529)
    {
        lcd_dev.p_height = LCD_HEIGHT;
        lcd_dev.p_width  = LCD_WDITH;
        lcd_dev.wramcmd  = 0x2C;
        lcd_dev.setxcmd  = 0x2A;
        lcd_dev.setycmd  = 0x2B;
        lcd_dev.memory_acc_cont_cmd  = 0x36;
        lcd_dev.dir_hor_normal_data  = 0xA0;
        lcd_dev.dir_hor_reverse_data = 0x60;
        lcd_dev.dir_ver_normal_data  = 0x00;
        lcd_dev.dir_ver_reverse_data = 0xC0;
        
        lcd_dev.disp_on_cmd  = 0x29;
        lcd_dev.disp_off_cmd = 0x28;
        
        /* Ĭ��ʹ�ú��� */
        lcd_dev.height = lcd_dev.p_width;
        lcd_dev.width  = lcd_dev.p_height;
    }
}

/**
 * @func    lcd_init
 * @brief   ��lcd���г�ʼ������
 * @retva   ��
 */
void lcd_init(void)
{
    lcd_config();
#ifdef LCD_ID
    lcd_dev.id = LCD_ID;
#else
    lcd_dev.id = lcd_read_id();
#endif
	
#ifdef RT_USING_FINSH
    rt_kprintf("��ǰҺ����IDΪ0x%x\r\n",lcd_dev.id);
#endif
    if (lcd_dev.id == 0x61529)
    {
        lcd_config_loal();
        lcd_reset();
        lcd_delay(120); 
      
        /* config tft regval	*/
        lcd_write_comm(0xB0);
        lcd_write_data(0x04);
        
        lcd_write_comm(0x36);    
        lcd_write_data(0x00);    
        
        lcd_write_comm(0x3A);    
        lcd_write_data(0x55);   
        
        lcd_write_comm(0xB4);    
        lcd_write_data(0x00); 
        
        lcd_write_comm(0xC0);
        lcd_write_data(0x03);//0013
        lcd_write_data(0xDF);//480
        lcd_write_data(0x40);
        lcd_write_data(0x12);
        lcd_write_data(0x00);
        lcd_write_data(0x01);
        lcd_write_data(0x00);
        lcd_write_data(0x43);
        
        
        lcd_write_comm(0xC1);//frame frequency
        lcd_write_data(0x05);//BCn,DIVn[1:0
        lcd_write_data(0x2f);//RTNn[4:0] 
        lcd_write_data(0x08);// BPn[7:0]
        lcd_write_data(0x08);// FPn[7:0]
        lcd_write_data(0x00);
        
        
        
        lcd_write_comm(0xC4);
        lcd_write_data(0x63);
        lcd_write_data(0x00);
        lcd_write_data(0x08);
        lcd_write_data(0x08);
        
        lcd_write_comm(0xC8);//Gamma
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);//26
        lcd_write_data(0x30);//32 
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);
        lcd_write_data(0x30);
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        
        lcd_write_comm(0xC9);//Gamma
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);//26
        lcd_write_data(0x30);//32 
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);
        lcd_write_data(0x30);
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        
        lcd_write_comm(0xCA);//Gamma
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);//26
        lcd_write_data(0x30);//32 
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        lcd_write_data(0x06);
        lcd_write_data(0x0c);
        lcd_write_data(0x16);
        lcd_write_data(0x24);
        lcd_write_data(0x30);
        lcd_write_data(0x48);
        lcd_write_data(0x3d);
        lcd_write_data(0x28);
        lcd_write_data(0x20);
        lcd_write_data(0x14);
        lcd_write_data(0x0c);
        lcd_write_data(0x04);
        
        
        lcd_write_comm(0xD0);
        lcd_write_data(0x95);
        lcd_write_data(0x06);
        lcd_write_data(0x08);
        lcd_write_data(0x10);
        lcd_write_data(0x3f);
        
        
        lcd_write_comm(0xD1);
        lcd_write_data(0x02);
        lcd_write_data(0x28);
        lcd_write_data(0x28);
        lcd_write_data(0x40);
        
        lcd_write_comm(0xE1);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);   
        lcd_write_data(0x00);   
        
        lcd_write_comm(0xE2);    
        lcd_write_data(0x80);    
        
        lcd_write_comm(0x2A);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);    
        lcd_write_data(0x01);    
        lcd_write_data(0x3F);    
        
        lcd_write_comm(0x2B);    
        lcd_write_data(0x00);    
        lcd_write_data(0x00);    
        lcd_write_data(0x01);    
        lcd_write_data(0xDF);    
        
        lcd_write_comm(0x11);
        
        lcd_delay(120);
        
        lcd_write_comm(0x29);
        
        lcd_write_comm(0xC1);//frame frequency
        lcd_write_data(0x05);//BCn,DIVn[1:0]
        lcd_write_data(0x2f);//RTNn[4:0] 
        lcd_write_data(0x08);// BPn[7:0]
        lcd_write_data(0x08);// FPn[7:0]
        lcd_write_data(0x00);
      
        lcd_write_comm(0x20);
        
        /**	�������Լ���ʾ��������
         * bit D7-page address order	0�Ļ���������������ͬ��1�Ļ������������µߵ�
         * bit D6-column address order 0�Ļ���������������ͬ��1�Ļ������������ҵߵ�
         * bit D5-page/column addressing order 0�Ļ�������1�Ļ����е���
         * bit D4-display device line refresh order 	Ԥ����Ĭ������Ϊ0
         * bit D3-RGB/BGR order 0ΪRGB��1ΪBGR
         * bit D2-display data latch order Ԥ����Ĭ������Ϊ0
         * bit D1-flip horizontal Ԥ�� Ĭ������Ϊ0
         * bit D0-flip vertic 0������1���¶Ե�
         */
        lcd_write_comm(0x36);    
        lcd_write_data(0xA0); 
        
        lcd_wr_ram_prepare();
        /* �������� */
        
        TIM1_PWM_Init(500, 168);
//        LCD_LIGHT_ON;
        
        TIM_SetCompare4(TIM1, 499);	//�޸ıȽ�ֵ���޸�ռ�ձ�
        
        lcd_set_disp_dir(DIR_HORIZONTAL_NORMAL);
        
        lcd_clr_scr(0x0000);
        
#ifdef RT_USING_FINSH
        rt_kprintf("R61529�������óɹ���\r\n");
#endif
    }
}

/**
 * @func    lcd_read_id
 * @brief   ��ȡLCD��ID
 * @retval  LCD��ID
 */
static uint32_t lcd_read_id(void)
{
    uint16_t buf[4];
    
    LCD_OPERATION->REG = 0x04;
    buf[0] = LCD_OPERATION->RAM;
    buf[1] = LCD_OPERATION->RAM;
    buf[2] = LCD_OPERATION->RAM;
    buf[3] = LCD_OPERATION->RAM;
    
    return (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

/**
 * @func    lcd_set_disp_dir
 * @brief   ����LCD��ɨ�跽��
 * @param   dir ��Ҫ���õķ���
 * @note    �������Լ���ʾ��������
 *            bit D7-page address order	0�Ļ���������������ͬ��1�Ļ������������µߵ�
 *            bit D6-column address order 0�Ļ���������������ͬ��1�Ļ������������ҵߵ�
 *            bit D5-page/column addressing order 0�Ļ�������1�Ļ����е���
 *            bit D4-display device line refresh order 	Ԥ����Ĭ������Ϊ0
 *            bit D3-RGB/BGR order 0ΪRGB��1ΪBGR
 *            bit D2-display data latch order Ԥ����Ĭ������Ϊ0
 *            bit D1-flip horizontal Ԥ�� Ĭ������Ϊ0
 *            bit D0-flip vertic 0������1���¶Ե�
 *            
 *            ���� �������������£���
 *            ���� ���������������£���
 * @retval  ��
 */
void lcd_set_disp_dir(uint8_t dir)
{
    lcd_dev.dir = dir;
    lcd_write_comm(lcd_dev.memory_acc_cont_cmd);
    
    if (dir == DIR_HORIZONTAL_NORMAL)
    {
        lcd_write_data(lcd_dev.dir_hor_normal_data);	
        lcd_dev.height = lcd_dev.p_width;
        lcd_dev.width  = lcd_dev.p_height;
#ifdef RT_USING_FINSH
        rt_kprintf("����Ϊ����\r\n");
#endif
    }
    else if (dir == DIR_HORIZONTAL_REVERSE)
    {
        lcd_write_data(lcd_dev.dir_hor_reverse_data);
        lcd_dev.height = lcd_dev.p_width;
        lcd_dev.width  = lcd_dev.p_height;
#ifdef RT_USING_FINSH
        rt_kprintf("����Ϊ����\r\n");
#endif
    } 
    else if (dir == DIR_VERTICAL_NORMAL)
    {
        lcd_write_data(lcd_dev.dir_ver_normal_data);
        lcd_dev.height = lcd_dev.p_height;
        lcd_dev.width  = lcd_dev.p_width;
#ifdef RT_USING_FINSH
        rt_kprintf("����Ϊ����\r\n");
#endif
    }
    else if (dir == DIR_VERTICAL_REVERSE)
    {
        lcd_write_data(lcd_dev.dir_ver_reverse_data);
        lcd_dev.height = lcd_dev.p_height;
        lcd_dev.width  = lcd_dev.p_width;
#ifdef RT_USING_FINSH
        rt_kprintf("����Ϊ����\r\n");
#endif
    }
}

/**
 * @func    lcd_set_disp_win
 * @brief   ѡ��Lcd��ָ���ľ������򣬼�����һ������
 * @param   cur_x x�������ʼ��
 * @param   cur_y y�������ʼ��
 * @param   w_width  ���ڵĿ��
 * @param   w_height ���ڵĸ߶�
 * @retval  ��
 */
void lcd_set_disp_win(uint16_t cur_x, uint16_t cur_y, uint16_t w_width, uint16_t w_height)
{
    /* �趨X���� */
    lcd_write_comm(lcd_dev.setxcmd);
    lcd_write_data(cur_x >> 8);
    lcd_write_data(0xFF & cur_x);                  
    lcd_write_data((cur_x + w_width - 1) >> 8);
    lcd_write_data(0xFF & (cur_x + w_width - 1));
    
    /* �趨Y���� */
    lcd_write_comm(lcd_dev.setycmd);
    lcd_write_data(cur_y >> 8);
    lcd_write_data(0xFF & cur_y);
    lcd_write_data((cur_y + w_height - 1) >> 8);
    lcd_write_data(0xFF & (cur_y + w_height - 1));
    
    /* ���Դ� */
    lcd_wr_ram_prepare();
}

uint16_t lcd_get_x_size(void)
{
    return lcd_dev.width;
}

uint16_t lcd_get_y_size(void)
{
    return lcd_dev.height;
}
/**
 * @func    lcd_set_disp_cur
 * @brief   ��Lcd��ָ����λ�����ù��
 * @param   x_pos x�������ʼ��
 * @param   y_pos y�������ʼ��
 * @retval  ��
 */
void lcd_set_disp_cur(uint16_t x_pos, uint16_t y_pos)
{
    pos_typedef pos;
    
    pos.pos = x_pos;
    
    /* �趨X���� */
    LCD_OPERATION->REG = lcd_dev.setxcmd;
    LCD_OPERATION->RAM = pos.Pos.h_bit;
    LCD_OPERATION->RAM = pos.Pos.l_bit;
    LCD_OPERATION->RAM = 0x01;
    LCD_OPERATION->RAM = 0xDF;
    
//    lcd_write_comm(lcd_dev.setxcmd);
//    lcd_write_data(x_pos >> 8);
//    lcd_write_data(0xFF & x_pos);
//    lcd_write_data(0x01);
//    lcd_write_data(0xDF);
    
    pos.pos = y_pos;
    
    /* �趨Y���� */
    LCD_OPERATION->REG = lcd_dev.setycmd;
    LCD_OPERATION->RAM = pos.Pos.h_bit;
    LCD_OPERATION->RAM = pos.Pos.l_bit;
    LCD_OPERATION->RAM = 0x01;
    LCD_OPERATION->RAM = 0x3F;
    
//    lcd_write_comm(lcd_dev.setycmd);
//    lcd_write_data(y_pos >> 8);
//    lcd_write_data(0xFF & y_pos);                 
//    lcd_write_data(0x01);
//    lcd_write_data(0x3f);
    
    /* ���Դ� */
    LCD_OPERATION->REG = lcd_dev.wramcmd;
//    lcd_wr_ram_prepare();
}

/**
 * @func    lcd_draw_colorbox
 * @brief   ѡ��Lcd��ָ���ľ������������ָ������ɫ
 * @param   cur_x x�������ʼ��
 * @param   cur_y y�������ʼ��
 * @param   x_size  Ҫѡ�����ε�x�����С
 * @param   y_size  Ҫѡ�����ε�y�����С
 * @param   p_color Ҫ������ɫ
 * @retval  ��
 */
void lcd_draw_colorbox(uint16_t cur_x, uint16_t cur_y, uint16_t x_size, uint16_t y_size, uint16_t p_color)
{
    uint32_t Temp;
    
    /* ���ô��� */
    lcd_set_disp_win(cur_x, cur_y, cur_x + x_size, cur_y + y_size);
    
    /* �����С��ѭ��д���� */
    Temp = x_size * y_size;
    while (Temp--)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_refresh_colorbox
 * @brief   ѡ��Lcd��ָ���ľ������������ָ������ɫ
 * @param   x_start x�������ʼ��
 * @param   y_srart y�������ʼ��
 * @param   x_end  Ҫѡ�����ε�x�����С
 * @param   y_end  Ҫѡ�����ε�y�����С
 * @param   p_color Ҫ������ɫ
 * @retval  ��
 */
void lcd_refresh_colorbox(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t p_color)
{	
    uint16_t i = 0, j = 0;
    
    /* ���ô��� */
    lcd.set_disp_win(x_start, y_start, x_end, y_end);
    
    /* ѭ��д���� */
    for (i = 0; i < x_end - x_start; i++)
    {
        for (j = 0; j < y_end - y_start; j++)
        {
            LCD_OPERATION->RAM = p_color;
        }
    }
}

/**
 * @func    lcd_gbr_to_rgb
 * @brief   �����ظ�ʽ����ת��
 * @param   color Ҫִ��ת��������
 * @retval  rgb ת����ɵ�����
 */
uint16_t lcd_gbr_to_rgb(uint16_t color)
{
    uint16_t  r,g,b,rgb;   
    
    b = (color >> 0) & 0x1f;
    g = (color >> 5) & 0x3f;
    r = (color >> 11) & 0x1f;
    
    rgb = (b << 11) + (g << 5) + (r << 0);		 
    
    return(rgb);
} 

/**
 * @func    lcd_quit_win_mode
 * @brief   �˳�������ʾģʽ����Ϊȫ����ʾģʽ
 * @retval  ��
 */
static void lcd_quit_win_mode(void)
{
    lcd_set_disp_win(0, 0, lcd_dev.width, lcd_dev.height);
}

/**
 * @func    lcd_disp_on
 * @brief   ����ʾ
 * @retval  ��
 */
void lcd_disp_on(void)
{
    lcd_write_comm(lcd_dev.disp_on_cmd);
}

/**
 * @func    lcd_disp_off
 * @brief   �ر���ʾ
 * @retval  ��
 */
void lcd_disp_off(void)
{
    lcd_write_comm(lcd_dev.disp_off_cmd);
}

/**
 * @func    lcd_clr_scr
 * @brief   �����������ɫֵ����
 * @param   p_color ����ɫ
 * @retval  ��
 */
void lcd_clr_scr(uint16_t p_color)
{
    uint32_t i;
    uint32_t n;
    
    lcd_set_disp_win(0, 0, lcd_dev.width, lcd_dev.height);
    
#if 1		/* �Ż�����ִ���ٶ� */
    n = (lcd_dev.width * lcd_dev.height) / 8;
    for (i = 0; i < n; i++)
    {
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
      
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
        LCD_OPERATION->RAM = p_color;
    }
#else
    n = lcd_dev.width * lcd_dev.height;
    while (n--)
        LCD_OPERATION->RAM = p_color;
#endif
}

/**
 * @func	lcd_put_pixel
 * @brief 	��1������
 * @param	cur_x ����x����
 * @param	cur_y ����y����
 * @param	p_color ������ɫ
 * @retval	��
 */
void lcd_put_pixel(uint16_t cur_x, uint16_t cur_y, uint16_t p_color)
{
	lcd_set_disp_cur(cur_x, cur_y);	/* ���ù��λ�� */
	
	LCD_OPERATION->RAM = p_color;
}

/**
 * @func    lcd_get_pixel
 * @brief   ��ȡ1������
 * @param   cur_x ����x����
 * @param   cur_y ����y����
 * @retval  ���������ص����ɫ
 */
uint16_t lcd_get_pixel(uint16_t cur_x, uint16_t cur_y)
{
    uint16_t R = 0, G = 0, B = 0 ;
    
    lcd_set_disp_cur(cur_x, cur_y);	/* ���ù��λ�� */
    
    LCD_OPERATION->REG = 0x2E;
    R = LCD_OPERATION->RAM; 	/* ��1���ƶ������� */
    R = LCD_OPERATION->RAM;
    B = LCD_OPERATION->RAM;
    G = LCD_OPERATION->RAM;
    
    return (((R >> 11) << 11) | ((G >> 10 ) << 5) | (B >> 11));
}

/**
 * @func    lcd_draw_line
 * @brief   ���� Bresenham �㷨����2��仭һ��ֱ�ߡ�
 * @param   x_start ��ʼ��x����
 * @param   y_start ��ʼ��y����
 * @param   x_end ��ֹ��x����
 * @param   y_end ��ֹ��y����
 * @param   p_color ������ɫ
 * @retval  ��
 */
void lcd_draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t p_color)
{
    int32_t dx, dy;
    int32_t tx, ty;
    int32_t inc1, inc2;
    int32_t d, iTag;
    int32_t x, y;
    
    /* ���� Bresenham �㷨����2��仭һ��ֱ�� */
    lcd_put_pixel(x_start, y_start, p_color);
    
    /* ��������غϣ���������Ķ�����*/
    if (x_start == x_end && y_start == y_end)
    {
        return;
    }
    
    iTag = 0 ;
    
    /* dx = abs ( _usX2 - _usX1 ); */
    if (x_end >= x_start)
    {
        dx = x_end - x_start;
    }
    else
    {
        dx = x_start - x_end;
    }
    
    /* dy = abs ( _usY2 - _usY1 ); */
    if (y_end >= y_start)
    {
        dy = y_end - y_start;
    }
    else
    {
        dy = y_start - y_end;
    }
    
    /*���dyΪ�Ƴ������򽻻��ݺ����ꡣ*/
    if (dx < dy)   
    {
        uint16_t temp;
      
        iTag = 1;
        temp = x_start; 
        x_start = y_start; 
        y_start = temp;
        temp = x_end; 
        x_end = y_end; 
        y_end = temp;
        temp = dx; 
        dx = dy; 
        dy = temp;
    } 
    
    /* ȷ������1���Ǽ�1 */
    tx = x_end > x_start ? 1 : -1;   
    ty = y_end > y_start ? 1 : -1;
    x = x_start;
    y = y_start;
    inc1 = 2 * dy;
    inc2 = 2 * (dy - dx);
    d = inc1 - dx;
    
    /* ѭ������ */
    while (x != x_end)     
    {
        if (d < 0)
        {
            d += inc1;
        }
        else
        {
            y += ty;
            d += inc2;
        }
        if (iTag)
        {
            lcd_put_pixel(y, x, p_color);
        }
        else
        {
            lcd_put_pixel(x , y, p_color);
        }
        x += tx ;
    }
}

/**
 * @func    lcd_draw_hor_line
 * @brief   ����һ��ˮƽ��
 * @param   x_start ��ʼ��x����
 * @param   y_start ��ʼ��y����
 * @param   x_end ��ֹ��x����
 * @param   p_color ������ɫ
 * @retval  ��
 */
void lcd_draw_hor_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t p_color)
{
    uint16_t i;
	
    lcd_set_disp_win(x_start, y_start, x_end - x_start + 1, 1);
    
    /* д�Դ� */
    for (i = 0; i < x_end - x_start + 1; i++)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_draw_line
 * @brief   ����ֱƽ��
 * @param   x_start ��ʼ��x����
 * @param   y_start ��ʼ��y����
 * @param   y_end ��ֹ��y����
 * @param   p_color ������ɫ
 * @retval  ��
 */
void lcd_draw_ver_line(uint16_t x_start, uint16_t y_start, uint16_t y_end, uint16_t p_color)
{
    uint16_t i;
    
    lcd_set_disp_win(x_start, y_start, 1, y_end - y_start + 1);
    
    /* д�Դ� */
    for (i = 0; i < y_end - y_start + 1; i++)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_draw_hor_color_line
 * @brief   ����һ����ɫˮƽ��
 * @param   cur_x ��ʼ��x����
 * @param   cur_y ��ʼ��y����
 * @param   d_width ֱ�ߵĿ��
 * @param   *p_color ��ɫ������
 * @retval  ��
 */
void lcd_draw_hor_color_line(uint16_t cur_x , uint16_t cur_y, uint16_t d_width, const uint16_t *p_color)
{
    uint16_t i;
	
    lcd_set_disp_win(cur_x, cur_y, d_width, 1);
    
    /* д�Դ� */
    for (i = 0; i < d_width; i++)
    {
        LCD_OPERATION->RAM = *(p_color++);
    }
}

/**
 * @func    lcd_draw_hor_trans_line
 * @brief   ����һ����ɫ͸����ˮƽ��
 * @param   cur_x ��ʼ��x����
 * @param   cur_y ��ʼ��y����
 * @param   d_width ֱ�ߵĿ��
 * @param   *p_color ��ɫ������
 * @retval  ��
 */
void lcd_draw_hor_trans_line(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, const uint16_t *p_color)
{
    uint16_t i, j;
    uint16_t Index;
	
    lcd_set_disp_cur(cur_x, cur_y);
    
    for (i = 0,j = 0; i < d_width; i++, j++)
    {
        Index = *(p_color++);
        if (Index)
        {
            LCD_OPERATION->RAM = Index;
        }
        else
        {
            lcd_set_disp_cur(cur_x + j, cur_y);
            LCD_OPERATION->REG = 0x2C;
            LCD_OPERATION->RAM = Index;
        }
    }
}

/**
 * @func    lcd_draw_rect
 * @brief   ����ˮƽ���õľ��Ρ�
 * @param   cur_x ��ʼ��x����
 * @param   cur_y ��ʼ��y����
 * @param   d_width ���εĿ��
 * @param   d_height ���εĸ߶�
 * @param   p_color ������ɫ
 * @note    ---------------->---
           |(cur_x��cur_y)      |
           V                    V  d_height
           |                    |
           ---------------->---
                d_width
 * @retval  ��
 */
void lcd_draw_rect(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, uint16_t d_height, uint16_t p_olor)
{
    /* �� */
    lcd_draw_line(cur_x, cur_y, cur_x + d_width - 1, cur_y, p_olor);	
    
    /* �� */
    lcd_draw_line(cur_x, cur_y + d_height - 1, cur_x + d_width - 1, cur_y + d_height - 1, p_olor);	
    
    /* �� */
    lcd_draw_line(cur_x, cur_y, cur_x, cur_y + d_height - 1, p_olor);	
    
    /* �� */
    lcd_draw_line(cur_x + d_width - 1, cur_y, cur_x + d_width - 1, cur_y + d_height, p_olor);	
}

/**  
 * @func    lcd_fill_rect
 * @brief   �����Ρ�
 * @param   cur_x ��ʼ��x����
 * @param   cur_y ��ʼ��y����
 * @param   d_width ���εĿ��
 * @param   d_height ���εĸ߶�
 * @param   p_color ������ɫ
 * @note     ---------------->---
            |(cur_x��cur_y)      |
            V                    V  d_height
            |                    |
            ---------------->---
                d_width
 * @retval  ��
 */
void lcd_fill_rect(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, uint16_t d_height, uint16_t p_olor)
{
    uint32_t i;
    
    lcd_set_disp_win(cur_x, cur_y, d_width, d_height);
    
    for (i = 0; i < d_width * d_height; i++)
    {
        LCD_OPERATION->RAM = p_olor;
    }
}

/**
 * @func    lcd_draw_circle
 * @brief   ����һ��Բ���ʿ�Ϊ1������
 * @param   cur_x Բ��x����
 * @param   cur_y Բ��y����
 * @param   r_radius Բ�İ뾶
 * @param   p_color ������ɫ
 * @retval  ��
 */
void lcd_draw_circle(uint16_t cur_x, uint16_t cur_y, uint16_t r_radius, uint16_t p_color)
{
    int32_t  D;			/* Decision Variable */
    uint32_t  n_cur_x;		/* ��ǰ X ֵ */
    uint32_t  n_cur_y;		/* ��ǰ Y ֵ */
    
    D = 3 - (r_radius << 1);
    n_cur_x = 0;
    n_cur_y = r_radius;
    
    while (n_cur_x <= n_cur_y)
    {
        lcd_put_pixel(cur_x + n_cur_x, cur_y + n_cur_y, p_color);
        lcd_put_pixel(cur_x + n_cur_x, cur_y - n_cur_y, p_color);
        lcd_put_pixel(cur_x - n_cur_x, cur_y + n_cur_y, p_color);
        lcd_put_pixel(cur_x - n_cur_x, cur_y - n_cur_y, p_color);
        lcd_put_pixel(cur_x + n_cur_y, cur_y + n_cur_x, p_color);
        lcd_put_pixel(cur_x + n_cur_y, cur_y - n_cur_x, p_color);
        lcd_put_pixel(cur_x - n_cur_y, cur_y + n_cur_x, p_color);
        lcd_put_pixel(cur_x - n_cur_y, cur_y - n_cur_x, p_color);
      
        if (D < 0)
        {
            D += (n_cur_x << 2) + 6;
        }
        else
        {
            D += ((n_cur_x - n_cur_y) << 2) + 10;
            n_cur_y--;
        }
        n_cur_x++;
    }
}

/**
 * @func    lcd_draw_bmp
 * @brief   ��LCD����ʾһ��BMPλͼ��λͼ����ɨ����򣺴����ң����ϵ���
 * @param   cur_x ��ʼ��x����
 * @param   cur_y ��ʼ��y����
 * @param   d_width ͼƬ���
 * @param   d_height ͼƬ�߶�
 * @param   *ptr ͼƬ����ָ��
 * @retval  ��
 */
void lcd_draw_bmp(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, uint16_t d_height, uint16_t *ptr)
{
    uint32_t index = 0;
    const uint16_t *p;
  
    /* ����ͼƬ��λ�úʹ�С�� ��������ʾ���� */
    lcd_set_disp_win(cur_x, cur_y, d_width, d_height);
    
    p = ptr;
    for (index = 0; index < d_height * d_width; index++)
    {
        LCD_OPERATION->RAM = *(p++);
    }
    
    /* �˳����ڻ�ͼģʽ */
    lcd_quit_win_mode();
}

/**
 * @func    lcd_func_init
 * @brief   LCD������Ա��ʼ��
 * @retval  ��
 */
void lcd_func_init(void)
{
    lcd.clr_scr = &lcd_clr_scr;
    lcd.disp_off = &lcd_disp_off;
    lcd.disp_on = &lcd_disp_on;
    lcd.get_x_size = &lcd_get_x_size;
    lcd.get_y_size = &lcd_get_y_size;
    lcd.draw_bmp = &lcd_draw_bmp;
    lcd.draw_circle = &lcd_draw_circle;
    lcd.draw_colorbox = &lcd_draw_colorbox;
    lcd.draw_hor_color_line = &lcd_draw_hor_color_line;
    lcd.draw_hor_line = &lcd_draw_hor_line;
    lcd.draw_hor_trans_line = &lcd_draw_hor_trans_line;
    lcd.draw_line = &lcd_draw_line;
    lcd.draw_rect = &lcd_draw_rect;
    lcd.draw_ver_line = &lcd_draw_ver_line;
    lcd.fill_rect = &lcd_fill_rect;
    lcd.gbr_to_rgb = &lcd_gbr_to_rgb;
    lcd.get_pixel = &lcd_get_pixel;
    lcd.init = &lcd_init;
    lcd.put_pixel = &lcd_put_pixel;
    lcd.refresh_colorbox = &lcd_refresh_colorbox;
    lcd.set_disp_dir = &lcd_set_disp_dir;
    lcd.set_disp_cur = &lcd_set_disp_cur;
    lcd.set_disp_win = &lcd_set_disp_win;
#ifdef RT_USING_FINSH
    rt_kprintf("LCD��Ա������ʼ����ϣ�\r\n");
#endif
}






