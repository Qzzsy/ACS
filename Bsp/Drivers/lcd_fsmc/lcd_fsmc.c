/**
 ******************************************************************************
 * @file      lcd_fsmc.c
 * @author    门禁开发小组
 * @version   V1.0.3
 * @date      2018-01-26
 * @brief     这个文件是液晶初始化文件，在此文件内进行液晶的配置还有一些基本的操作
              液晶的驱动芯片为R61529驱动芯片
 * @History
 * Date           Author    version    		Notes
 * 2017-10-31       ZSY     V1.0.0      first version.
 * 2017-11-27       ZSY     V1.0.1      增加LCD的操作方法，完善LCD驱动
 * 2018-01-16       ZSY     V1.0.2      排版格式化，增强可视化
 * 2018-01-26       ZSY     V1.0.3      整理部分定义，添加私有和公有宏定义
 */
	
/* Includes ------------------------------------------------------------------*/
#include "lcd_fsmc.h"
#include "rtthread.h"
#include "pwm.h"

/* Private macro Definition --------------------------------------------------*/



/* 定义LCD复位操作的宏定义 */  
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
 * @brief   液晶FSMC硬件配置，配置液晶的FSMC数据线，时序和控制线以及FSMC本身
            的配置
 * @note    需要开启对应的时钟和复用
 * @retval  无
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
     * data line ，FSMC-D0-D15：PD 14 15 0 1 PE 7 8 9 10 11 12 13 14 15，PD 8,9,10
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
     * alternate function IO PD0，PD1，PD8，PD9，PD10，PD14，PD15 to FSMC output
     */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);		//PD0,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);		//PD1,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC); 	//PD8,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);		//PD9,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);	//PD10,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);	//PD14,AF12
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);	//PD15,AF12
    
    /** config tft FSMC AF line
     * alternate function IO PE7，PE8，PE9，PE10，PE11，PE12，PE13，PE14，PE15 to FSMC output
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
    
    readWriteTiming.FSMC_AddressSetupTime = 0x02;       //地址建立时间（ADDSET）为1个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime  = 0x00;       //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime    = 0x0A;       //数据保持时间（DATAST）为9个HCLK 6*9=54ns	 
    
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x01;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode  = FSMC_AccessMode_A;	 //模式A 
    
    /**	Configuring FSMC Write Timings
     * Address Setup time is 9 HCLK = 54ns 			地址建立时间
     * Address Hold time												地址保持时间
     * Data Setup time is 6ns*9 a hclk = 54ns		数据保存时间
     */
    writeTiming.FSMC_AddressSetupTime = 8;	      					
    writeTiming.FSMC_AddressHoldTime  = 0x00;	 							
    writeTiming.FSMC_DataSetupTime    = 8;		 								
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;					
    writeTiming.FSMC_CLKDivision = 0x01;									
    writeTiming.FSMC_DataLatency = 0x00;										
    writeTiming.FSMC_AccessMode  = FSMC_AccessMode_A;	 			/* mode B */
    
    /* config tft control base for FSMC	*/
    FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM4;                 //这里我们使用NE1，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;         //不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_SRAM;                //FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;            //存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;        //FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;          //存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Enable;            //读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;                    //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &writeTiming;                        //写时序
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                                               //初始化FSMC配置
    
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
 * @brief   lcd写命令到寄存器
 * @param   regval 指定的寄存器地址
 * @retval  无
 */
static void lcd_write_comm(__IO uint16_t  regval)
{   
    LCD_OPERATION->REG = regval;			/* 写入要写的寄存器序号 */
}

/**
 * @func    lcd_write_comm
 * @brief   lcd写数据
 * @param   data 要写入的数据
 * @retval  无
 */
static void lcd_write_data(__IO uint16_t  data)
{	  
    LCD_OPERATION->RAM = data;		 
}		

/**
 * @func    lcd_wr_reg
 * @brief   往指定的寄存器写数据
 * @param   index 寄存器
 * @param   congfig_temp 数据
 * @retval  无
 */
static void lcd_wr_reg(uint16_t index, uint16_t congfig_temp)
{
    LCD_OPERATION->REG = index;
    LCD_OPERATION->REG = congfig_temp;
}

/**
 * @func    lcd_read_data
 * @brief   lcd读数据
 * @retval  读到的数据
 */
static uint16_t lcd_read_data(void)
{
    vu16 ram;			//防止被优化
    ram = LCD_OPERATION->RAM;	
    return ram;	 
}	

/**
 * @func    lcd_read_reg
 * @brief   lcd读寄存器
 * @param   reg:寄存器地址
 * @retval  读到的数据
 */
static uint16_t lcd_read_reg(uint16_t reg)
{										   
    lcd_write_comm(reg);		//写入要读的寄存器序号
    return LCD_OPERATION->RAM;		//返回读到的值
}  

/**
 * @func    lcd_wr_ram_prepare
 * @brief   lcd开始写入操作
 * @note    一般在操作结束时调用
 * @retval  无
 */
static void lcd_wr_ram_prepare(void)
{
    LCD_OPERATION->REG = lcd_dev.wramcmd;
}

/**
 * @func    lcd_delay
 * @brief   lcd启动延时，用于初始化过程
 * @param   n_count 延时的大小
 * @retval  无
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
 * @brief   lcd复位操作，用于复位lcd的所有寄存器
 * @note    要延时足够的时间才能让lcd复位
 * @retval  无
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
 * @brief   根据不同的LCD加载不同的参数
 * @retval  无
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
        
        /* 默认使用横屏 */
        lcd_dev.height = lcd_dev.p_width;
        lcd_dev.width  = lcd_dev.p_height;
    }
}

/**
 * @func    lcd_init
 * @brief   对lcd进行初始化配置
 * @retva   无
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
    rt_kprintf("当前液晶的ID为0x%x\r\n",lcd_dev.id);
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
        
        /**	横竖屏以及显示方向设置
         * bit D7-page address order	0的话输入和输出方向相同，1的话输入和输出上下颠倒
         * bit D6-column address order 0的话输出和输出方向相同，1的话输出和输出左右颠倒
         * bit D5-page/column addressing order 0的话正常，1的话行列调换
         * bit D4-display device line refresh order 	预留，默认设置为0
         * bit D3-RGB/BGR order 0为RGB，1为BGR
         * bit D2-display data latch order 预留，默认设置为0
         * bit D1-flip horizontal 预留 默认设置为0
         * bit D0-flip vertic 0正常，1上下对调
         */
        lcd_write_comm(0x36);    
        lcd_write_data(0xA0); 
        
        lcd_wr_ram_prepare();
        /* 开启背光 */
        
        TIM1_PWM_Init(500, 168);
//        LCD_LIGHT_ON;
        
        TIM_SetCompare4(TIM1, 499);	//修改比较值，修改占空比
        
        lcd_set_disp_dir(DIR_HORIZONTAL_NORMAL);
        
        lcd_clr_scr(0x0000);
        
#ifdef RT_USING_FINSH
        rt_kprintf("R61529驱动配置成功！\r\n");
#endif
    }
}

/**
 * @func    lcd_read_id
 * @brief   读取LCD的ID
 * @retval  LCD的ID
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
 * @brief   设置LCD的扫描方向
 * @param   dir 需要设置的方向
 * @note    横竖屏以及显示方向设置
 *            bit D7-page address order	0的话输入和输出方向相同，1的话输入和输出上下颠倒
 *            bit D6-column address order 0的话输出和输出方向相同，1的话输出和输出左右颠倒
 *            bit D5-page/column addressing order 0的话正常，1的话行列调换
 *            bit D4-display device line refresh order 	预留，默认设置为0
 *            bit D3-RGB/BGR order 0为RGB，1为BGR
 *            bit D2-display data latch order 预留，默认设置为0
 *            bit D1-flip horizontal 预留 默认设置为0
 *            bit D0-flip vertic 0正常，1上下对调
 *            
 *            横屏 正常是排线在下！！
 *            竖屏 正常是排线在右下！！
 * @retval  无
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
        rt_kprintf("设置为横屏\r\n");
#endif
    }
    else if (dir == DIR_HORIZONTAL_REVERSE)
    {
        lcd_write_data(lcd_dev.dir_hor_reverse_data);
        lcd_dev.height = lcd_dev.p_width;
        lcd_dev.width  = lcd_dev.p_height;
#ifdef RT_USING_FINSH
        rt_kprintf("设置为横屏\r\n");
#endif
    } 
    else if (dir == DIR_VERTICAL_NORMAL)
    {
        lcd_write_data(lcd_dev.dir_ver_normal_data);
        lcd_dev.height = lcd_dev.p_height;
        lcd_dev.width  = lcd_dev.p_width;
#ifdef RT_USING_FINSH
        rt_kprintf("设置为竖屏\r\n");
#endif
    }
    else if (dir == DIR_VERTICAL_REVERSE)
    {
        lcd_write_data(lcd_dev.dir_ver_reverse_data);
        lcd_dev.height = lcd_dev.p_height;
        lcd_dev.width  = lcd_dev.p_width;
#ifdef RT_USING_FINSH
        rt_kprintf("设置为竖屏\r\n");
#endif
    }
}

/**
 * @func    lcd_set_disp_win
 * @brief   选定Lcd上指定的矩形区域，即设置一个窗口
 * @param   cur_x x方向的起始点
 * @param   cur_y y方向的起始点
 * @param   w_width  窗口的宽度
 * @param   w_height 窗口的高度
 * @retval  无
 */
void lcd_set_disp_win(uint16_t cur_x, uint16_t cur_y, uint16_t w_width, uint16_t w_height)
{
    /* 设定X坐标 */
    lcd_write_comm(lcd_dev.setxcmd);
    lcd_write_data(cur_x >> 8);
    lcd_write_data(0xFF & cur_x);                  
    lcd_write_data((cur_x + w_width - 1) >> 8);
    lcd_write_data(0xFF & (cur_x + w_width - 1));
    
    /* 设定Y坐标 */
    lcd_write_comm(lcd_dev.setycmd);
    lcd_write_data(cur_y >> 8);
    lcd_write_data(0xFF & cur_y);
    lcd_write_data((cur_y + w_height - 1) >> 8);
    lcd_write_data(0xFF & (cur_y + w_height - 1));
    
    /* 开显存 */
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
 * @brief   在Lcd上指定的位置设置光标
 * @param   x_pos x方向的起始点
 * @param   y_pos y方向的起始点
 * @retval  无
 */
void lcd_set_disp_cur(uint16_t x_pos, uint16_t y_pos)
{
    pos_typedef pos;
    
    pos.pos = x_pos;
    
    /* 设定X坐标 */
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
    
    /* 设定Y坐标 */
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
    
    /* 开显存 */
    LCD_OPERATION->REG = lcd_dev.wramcmd;
//    lcd_wr_ram_prepare();
}

/**
 * @func    lcd_draw_colorbox
 * @brief   选定Lcd上指定的矩形区域并且填充指定的颜色
 * @param   cur_x x方向的起始点
 * @param   cur_y y方向的起始点
 * @param   x_size  要选定矩形的x方向大小
 * @param   y_size  要选定矩形的y方向大小
 * @param   p_color 要填充的颜色
 * @retval  无
 */
void lcd_draw_colorbox(uint16_t cur_x, uint16_t cur_y, uint16_t x_size, uint16_t y_size, uint16_t p_color)
{
    uint32_t Temp;
    
    /* 设置窗口 */
    lcd_set_disp_win(cur_x, cur_y, cur_x + x_size, cur_y + y_size);
    
    /* 计算大小并循环写数据 */
    Temp = x_size * y_size;
    while (Temp--)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_refresh_colorbox
 * @brief   选定Lcd上指定的矩形区域并且填充指定的颜色
 * @param   x_start x方向的起始点
 * @param   y_srart y方向的起始点
 * @param   x_end  要选定矩形的x方向大小
 * @param   y_end  要选定矩形的y方向大小
 * @param   p_color 要填充的颜色
 * @retval  无
 */
void lcd_refresh_colorbox(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t p_color)
{	
    uint16_t i = 0, j = 0;
    
    /* 设置窗口 */
    lcd.set_disp_win(x_start, y_start, x_end, y_end);
    
    /* 循环写数据 */
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
 * @brief   将像素格式进行转换
 * @param   color 要执行转换的像素
 * @retval  rgb 转换完成的像素
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
 * @brief   退出窗口显示模式，变为全屏显示模式
 * @retval  无
 */
static void lcd_quit_win_mode(void)
{
    lcd_set_disp_win(0, 0, lcd_dev.width, lcd_dev.height);
}

/**
 * @func    lcd_disp_on
 * @brief   打开显示
 * @retval  无
 */
void lcd_disp_on(void)
{
    lcd_write_comm(lcd_dev.disp_on_cmd);
}

/**
 * @func    lcd_disp_off
 * @brief   关闭显示
 * @retval  无
 */
void lcd_disp_off(void)
{
    lcd_write_comm(lcd_dev.disp_off_cmd);
}

/**
 * @func    lcd_clr_scr
 * @brief   根据输入的颜色值清屏
 * @param   p_color 背景色
 * @retval  无
 */
void lcd_clr_scr(uint16_t p_color)
{
    uint32_t i;
    uint32_t n;
    
    lcd_set_disp_win(0, 0, lcd_dev.width, lcd_dev.height);
    
#if 1		/* 优化代码执行速度 */
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
 * @brief 	画1个像素
 * @param	cur_x 像素x坐标
 * @param	cur_y 像素y坐标
 * @param	p_color 像素颜色
 * @retval	无
 */
void lcd_put_pixel(uint16_t cur_x, uint16_t cur_y, uint16_t p_color)
{
	lcd_set_disp_cur(cur_x, cur_y);	/* 设置光标位置 */
	
	LCD_OPERATION->RAM = p_color;
}

/**
 * @func    lcd_get_pixel
 * @brief   读取1个像素
 * @param   cur_x 像素x坐标
 * @param   cur_y 像素y坐标
 * @retval  读到的像素点的颜色
 */
uint16_t lcd_get_pixel(uint16_t cur_x, uint16_t cur_y)
{
    uint16_t R = 0, G = 0, B = 0 ;
    
    lcd_set_disp_cur(cur_x, cur_y);	/* 设置光标位置 */
    
    LCD_OPERATION->REG = 0x2E;
    R = LCD_OPERATION->RAM; 	/* 第1个哑读，丢弃 */
    R = LCD_OPERATION->RAM;
    B = LCD_OPERATION->RAM;
    G = LCD_OPERATION->RAM;
    
    return (((R >> 11) << 11) | ((G >> 10 ) << 5) | (B >> 11));
}

/**
 * @func    lcd_draw_line
 * @brief   采用 Bresenham 算法，在2点间画一条直线。
 * @param   x_start 起始点x坐标
 * @param   y_start 起始点y坐标
 * @param   x_end 终止点x坐标
 * @param   y_end 终止点y坐标
 * @param   p_color 画笔颜色
 * @retval  无
 */
void lcd_draw_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t p_color)
{
    int32_t dx, dy;
    int32_t tx, ty;
    int32_t inc1, inc2;
    int32_t d, iTag;
    int32_t x, y;
    
    /* 采用 Bresenham 算法，在2点间画一条直线 */
    lcd_put_pixel(x_start, y_start, p_color);
    
    /* 如果两点重合，结束后面的动作。*/
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
    
    /*如果dy为计长方向，则交换纵横坐标。*/
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
    
    /* 确定是增1还是减1 */
    tx = x_end > x_start ? 1 : -1;   
    ty = y_end > y_start ? 1 : -1;
    x = x_start;
    y = y_start;
    inc1 = 2 * dy;
    inc2 = 2 * (dy - dx);
    d = inc1 - dx;
    
    /* 循环画点 */
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
 * @brief   绘制一条水平线
 * @param   x_start 起始点x坐标
 * @param   y_start 起始点y坐标
 * @param   x_end 终止点x坐标
 * @param   p_color 画笔颜色
 * @retval  无
 */
void lcd_draw_hor_line(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t p_color)
{
    uint16_t i;
	
    lcd_set_disp_win(x_start, y_start, x_end - x_start + 1, 1);
    
    /* 写显存 */
    for (i = 0; i < x_end - x_start + 1; i++)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_draw_line
 * @brief   画垂直平线
 * @param   x_start 起始点x坐标
 * @param   y_start 起始点y坐标
 * @param   y_end 终止点y坐标
 * @param   p_color 画笔颜色
 * @retval  无
 */
void lcd_draw_ver_line(uint16_t x_start, uint16_t y_start, uint16_t y_end, uint16_t p_color)
{
    uint16_t i;
    
    lcd_set_disp_win(x_start, y_start, 1, y_end - y_start + 1);
    
    /* 写显存 */
    for (i = 0; i < y_end - y_start + 1; i++)
    {
        LCD_OPERATION->RAM = p_color;
    }
}

/**
 * @func    lcd_draw_hor_color_line
 * @brief   绘制一条彩色水平线
 * @param   cur_x 起始点x坐标
 * @param   cur_y 起始点y坐标
 * @param   d_width 直线的宽度
 * @param   *p_color 颜色缓冲区
 * @retval  无
 */
void lcd_draw_hor_color_line(uint16_t cur_x , uint16_t cur_y, uint16_t d_width, const uint16_t *p_color)
{
    uint16_t i;
	
    lcd_set_disp_win(cur_x, cur_y, d_width, 1);
    
    /* 写显存 */
    for (i = 0; i < d_width; i++)
    {
        LCD_OPERATION->RAM = *(p_color++);
    }
}

/**
 * @func    lcd_draw_hor_trans_line
 * @brief   绘制一条彩色透明的水平线
 * @param   cur_x 起始点x坐标
 * @param   cur_y 起始点y坐标
 * @param   d_width 直线的宽度
 * @param   *p_color 颜色缓冲区
 * @retval  无
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
 * @brief   绘制水平放置的矩形。
 * @param   cur_x 起始点x坐标
 * @param   cur_y 起始点y坐标
 * @param   d_width 矩形的宽度
 * @param   d_height 矩形的高度
 * @param   p_color 画笔颜色
 * @note    ---------------->---
           |(cur_x，cur_y)      |
           V                    V  d_height
           |                    |
           ---------------->---
                d_width
 * @retval  无
 */
void lcd_draw_rect(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, uint16_t d_height, uint16_t p_olor)
{
    /* 顶 */
    lcd_draw_line(cur_x, cur_y, cur_x + d_width - 1, cur_y, p_olor);	
    
    /* 底 */
    lcd_draw_line(cur_x, cur_y + d_height - 1, cur_x + d_width - 1, cur_y + d_height - 1, p_olor);	
    
    /* 左 */
    lcd_draw_line(cur_x, cur_y, cur_x, cur_y + d_height - 1, p_olor);	
    
    /* 右 */
    lcd_draw_line(cur_x + d_width - 1, cur_y, cur_x + d_width - 1, cur_y + d_height, p_olor);	
}

/**  
 * @func    lcd_fill_rect
 * @brief   填充矩形。
 * @param   cur_x 起始点x坐标
 * @param   cur_y 起始点y坐标
 * @param   d_width 矩形的宽度
 * @param   d_height 矩形的高度
 * @param   p_color 画笔颜色
 * @note     ---------------->---
            |(cur_x，cur_y)      |
            V                    V  d_height
            |                    |
            ---------------->---
                d_width
 * @retval  无
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
 * @brief   绘制一个圆，笔宽为1个像素
 * @param   cur_x 圆心x坐标
 * @param   cur_y 圆心y坐标
 * @param   r_radius 圆的半径
 * @param   p_color 画笔颜色
 * @retval  无
 */
void lcd_draw_circle(uint16_t cur_x, uint16_t cur_y, uint16_t r_radius, uint16_t p_color)
{
    int32_t  D;			/* Decision Variable */
    uint32_t  n_cur_x;		/* 当前 X 值 */
    uint32_t  n_cur_y;		/* 当前 Y 值 */
    
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
 * @brief   在LCD上显示一个BMP位图，位图点阵扫描次序：从左到右，从上到下
 * @param   cur_x 起始点x坐标
 * @param   cur_y 起始点y坐标
 * @param   d_width 图片宽度
 * @param   d_height 图片高度
 * @param   *ptr 图片点阵指针
 * @retval  无
 */
void lcd_draw_bmp(uint16_t cur_x, uint16_t cur_y, uint16_t d_width, uint16_t d_height, uint16_t *ptr)
{
    uint32_t index = 0;
    const uint16_t *p;
  
    /* 设置图片的位置和大小， 即设置显示窗口 */
    lcd_set_disp_win(cur_x, cur_y, d_width, d_height);
    
    p = ptr;
    for (index = 0; index < d_height * d_width; index++)
    {
        LCD_OPERATION->RAM = *(p++);
    }
    
    /* 退出窗口绘图模式 */
    lcd_quit_win_mode();
}

/**
 * @func    lcd_func_init
 * @brief   LCD方法成员初始化
 * @retval  无
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
    rt_kprintf("LCD成员方法初始化完毕！\r\n");
#endif
}






