/**
 ******************************************************************************
 * @file      iic.c
 * @author    门禁开发小组
 * @version   V1.0.3
 * @date      2018-01-26
 * @brief     文件内包含一些iic的引脚配置，iic的基本读写操作以及通信时序
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0          first version.
 * 2017-11-02     ZSY       V1.0.1          修改了应答信号的反馈形式，增加了对应的宏
                                            定义，对iic_ack和iic_no_ack添加了static
                                            的修饰符，仅限于本文件内使用
 * 2018-01-09     ZSY       V1.0.2          排版格式化操作.
 * 2018-01-26     ZSY       V1.0.3      添加私有和公有宏定义.
 */
	
/* Includes ------------------------------------------------------------------*/
#include "iic.h"
#include "delay.h"

/* Private macro Definition --------------------------------------------------*/
		   
/* 宏定义iic的硬件接口 */
#define IIC_SCL_PIN   GPIO_Pin_8
#define IIC_SDA_PIN   GPIO_Pin_9   

#define IIC_SCL_PORT	GPIOB
#define IIC_SDA_PORT	GPIOB

/* IO方向设置 */
#define SET_IIC_SDA_IN()  {IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); IIC_SDA_PORT->MODER |= (0 << (9 * 2));}	//PB12输入模式
#define SET_IIC_SDA_OUT() {IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); IIC_SDA_PORT->MODER |= (1 << (9 * 2));} 	//PB12输出模式

/* 设置iic接口的高低逻辑电平输出 */
#define IIC_SCL_WRITE_H   GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)
#define IIC_SCL_WRITE_L   GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)
						
#define IIC_SDA_WRITE_H   GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define IIC_SDA_WRITE_L   GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN)

/* 设置iic的sda线的读入功能	*/
#define	IIC_SDA_READ      GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

/* 应答信号ACK等待超时时间	*/
#define IIC_ACK_TIMEOUT   200

/* End private macro Definition ----------------------------------------------*/

/* global variable Declaration -----------------------------------------------*/



/* User function Declaration -------------------------------------------------*/



/* User functions ------------------------------------------------------------*/

/**
 * @func    iic_gpio_config
 * @brief   设置触屏的IIC引脚,用软件模拟的方法实现IIC功能
 * @note    需要开启对应的时钟和复用
 * @retval  无
 */
void iic_gpio_config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOB和GPIOC的时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		
    /**	config PIN Status	
     * alternate function IO output
     * push-Pull output
     * speed is 100MHz
     * pull output
    */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
    
    /* config iic scl line */
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;  		//PC5  
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
    
    /* config iic sda line */
    GPIO_InitStructure.GPIO_Pin =  IIC_SDA_PIN; 		//PC4
    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
    
    /* 初始化完成后设置为高 */
    IIC_SCL_WRITE_H;
    IIC_SDA_WRITE_H;
    
    /* iic stop transport */
    iic_stop();
}

/**
 * @func    iic_start
 * @brief   iic start transport
                IIC device is about to start a new transfer process
 * @retval  none
 */
void iic_start(void)  
{ 
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    /* iic produce a start signal */
    IIC_SDA_WRITE_H;
    IIC_SCL_WRITE_H;
    delay_us(4);
    IIC_SDA_WRITE_L;
    delay_us(4);
    IIC_SCL_WRITE_L; 
} 

/**
 * @func    iic_stop
 * @brief   iic stop transport
                IIC device is about to stop the current transport process
 * @retval  none
 */
void iic_stop(void)  
{ 
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    /* iic produce a stop signal */
    IIC_SDA_WRITE_L;
    IIC_SCL_WRITE_L;
    delay_us(4);
    IIC_SDA_WRITE_H;
    IIC_SCL_WRITE_H;
    delay_us(4);
}

/**
 * @func    iic_ack
 * @brief   iic master produce a ack signal
 * @retval  none
 */
static void iic_ack(void) 
{ 
    IIC_SCL_WRITE_L;
    
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    IIC_SDA_WRITE_L;
    delay_us(2);
    IIC_SCL_WRITE_H;
    delay_us(2);
    IIC_SCL_WRITE_L;
} 

/**
 * @func    iic_no_ack
 * @brief   iic slave don't produce a ack signal，transport will stop
 * @retval  none
 */
static void iic_no_ack(void)
{
    IIC_SCL_WRITE_L;
    
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    /* iic produce no ack signal */
    IIC_SDA_WRITE_H;
    delay_us(2);
    IIC_SCL_WRITE_H;
    delay_us(2);
    IIC_SCL_WRITE_L;	
}

/**
 * @func    iic_wait_ack
 * @brief   iic master waiting a ack signal from slave
                The CPU produces a clock and reads the device's ACK signal
 * @retval  return IIC_OPER_OK for correct response, IIC_OPER_FAILT for no device response
 */
uint8_t iic_wait_ack(void)
{ 
    __IO uint16_t time = 0;
    
    /* Set GPIO to input mode */
    SET_IIC_SDA_IN();
    
    IIC_SDA_WRITE_H;  
    delay_us(1);
    IIC_SCL_WRITE_H;
    delay_us(1);
    
    /* waiting slave ack signal	*/
    while (IIC_SDA_READ)
    {
        time++;
        
        /* 超时检测，防止卡死 */
        if (time > IIC_ACK_TIMEOUT)
        {
            iic_stop();
            
            /* 超时意味着失败 */
            return IIC_OPER_FAILT;
        }	
    }
    
    IIC_SCL_WRITE_L;
    
    /* 有应答信号说明成功 */
    return IIC_OPER_OK; 
}

/**
 * @func    iic_send_byte
 * @brief   iic 发送一个字节的数据
 * @param   dat 将要发送的数据					
 * @retval  无
 */
void iic_send_byte(uint8_t data)
{
    __IO uint8_t i;
    
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    IIC_SCL_WRITE_L;
    
    /* 循环发送一个字节的数据 */
    for (i = 0; i < 8; i++)
    {		
        if (data & 0x80)
        {
            IIC_SDA_WRITE_H;
        }
        else
        {
            IIC_SDA_WRITE_L;
        }
        delay_us(2);
        IIC_SCL_WRITE_H;	
        delay_us(2);
        IIC_SCL_WRITE_L;
        delay_us(2);
        data <<= 1;	
    }
    IIC_SDA_WRITE_H;
}

/**
 * @func    iic_read_byte
 * @brief   iic 从设备读取一个字节的数据
 * @param   ack 主机应答的类型
                IIC_NEED_ACK 说明当前还不是最后一个字节，此时发送 ack
                IIC_NEEDNT_ACK 说明当前的传输是最后一个字节的数据，此时发送nack
 * @retval	receive 读取到的数据
 */
uint8_t iic_read_byte(uint8_t ack)
{
    unsigned char i,receive=0;
    
    /* Set Gpio to input mode */
    SET_IIC_SDA_IN();
    
    IIC_SDA_WRITE_H;
    
    /* 循环读取一个字节的数据 */
    for (i = 0; i < 8; i++ )
    {
        IIC_SCL_WRITE_L; 
        delay_us(2);
        IIC_SCL_WRITE_H;
        receive <<= 1;
        if (IIC_SDA_READ)
        {
            receive++;   
        }
        delay_us(2); 
    }		
    
    if (ack == IIC_NEEDNT_ACK) 
    {	   
        iic_no_ack();	//发送nACK
    }
    else       
    {
        iic_ack(); 		//发送ACK   
    }
      
    return receive;
}



