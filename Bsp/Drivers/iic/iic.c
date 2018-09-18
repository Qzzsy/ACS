/**
 ******************************************************************************
 * @file      iic.c
 * @author    �Ž�����С��
 * @version   V1.0.3
 * @date      2018-01-26
 * @brief     �ļ��ڰ���һЩiic���������ã�iic�Ļ�����д�����Լ�ͨ��ʱ��
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0          first version.
 * 2017-11-02     ZSY       V1.0.1          �޸���Ӧ���źŵķ�����ʽ�������˶�Ӧ�ĺ�
                                            ���壬��iic_ack��iic_no_ack�����static
                                            �����η��������ڱ��ļ���ʹ��
 * 2018-01-09     ZSY       V1.0.2          �Ű��ʽ������.
 * 2018-01-26     ZSY       V1.0.3      ���˽�к͹��к궨��.
 */
	
/* Includes ------------------------------------------------------------------*/
#include "iic.h"
#include "delay.h"

/* Private macro Definition --------------------------------------------------*/
		   
/* �궨��iic��Ӳ���ӿ� */
#define IIC_SCL_PIN   GPIO_Pin_8
#define IIC_SDA_PIN   GPIO_Pin_9   

#define IIC_SCL_PORT	GPIOB
#define IIC_SDA_PORT	GPIOB

/* IO�������� */
#define SET_IIC_SDA_IN()  {IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); IIC_SDA_PORT->MODER |= (0 << (9 * 2));}	//PB12����ģʽ
#define SET_IIC_SDA_OUT() {IIC_SDA_PORT->MODER &= ~(3 << (9 * 2)); IIC_SDA_PORT->MODER |= (1 << (9 * 2));} 	//PB12���ģʽ

/* ����iic�ӿڵĸߵ��߼���ƽ��� */
#define IIC_SCL_WRITE_H   GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)
#define IIC_SCL_WRITE_L   GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)
						
#define IIC_SDA_WRITE_H   GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define IIC_SDA_WRITE_L   GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN)

/* ����iic��sda�ߵĶ��빦��	*/
#define	IIC_SDA_READ      GPIO_ReadInputDataBit(IIC_SDA_PORT,IIC_SDA_PIN)

/* Ӧ���ź�ACK�ȴ���ʱʱ��	*/
#define IIC_ACK_TIMEOUT   200

/* End private macro Definition ----------------------------------------------*/

/* global variable Declaration -----------------------------------------------*/



/* User function Declaration -------------------------------------------------*/



/* User functions ------------------------------------------------------------*/

/**
 * @func    iic_gpio_config
 * @brief   ���ô�����IIC����,�����ģ��ķ���ʵ��IIC����
 * @note    ��Ҫ������Ӧ��ʱ�Ӻ͸���
 * @retval  ��
 */
void iic_gpio_config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��GPIOB��GPIOC��ʱ�� */
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
    
    /* ��ʼ����ɺ�����Ϊ�� */
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
 * @brief   iic slave don't produce a ack signal��transport will stop
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
        
        /* ��ʱ��⣬��ֹ���� */
        if (time > IIC_ACK_TIMEOUT)
        {
            iic_stop();
            
            /* ��ʱ��ζ��ʧ�� */
            return IIC_OPER_FAILT;
        }	
    }
    
    IIC_SCL_WRITE_L;
    
    /* ��Ӧ���ź�˵���ɹ� */
    return IIC_OPER_OK; 
}

/**
 * @func    iic_send_byte
 * @brief   iic ����һ���ֽڵ�����
 * @param   dat ��Ҫ���͵�����					
 * @retval  ��
 */
void iic_send_byte(uint8_t data)
{
    __IO uint8_t i;
    
    /* Set GPIO to Output mode */
    SET_IIC_SDA_OUT();
    
    IIC_SCL_WRITE_L;
    
    /* ѭ������һ���ֽڵ����� */
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
 * @brief   iic ���豸��ȡһ���ֽڵ�����
 * @param   ack ����Ӧ�������
                IIC_NEED_ACK ˵����ǰ���������һ���ֽڣ���ʱ���� ack
                IIC_NEEDNT_ACK ˵����ǰ�Ĵ��������һ���ֽڵ����ݣ���ʱ����nack
 * @retval	receive ��ȡ��������
 */
uint8_t iic_read_byte(uint8_t ack)
{
    unsigned char i,receive=0;
    
    /* Set Gpio to input mode */
    SET_IIC_SDA_IN();
    
    IIC_SDA_WRITE_H;
    
    /* ѭ����ȡһ���ֽڵ����� */
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
        iic_no_ack();	//����nACK
    }
    else       
    {
        iic_ack(); 		//����ACK   
    }
      
    return receive;
}



