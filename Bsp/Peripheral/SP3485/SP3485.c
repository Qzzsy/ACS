/**
 ******************************************************************************
 * @file      sp3485.c
 * @author    �Ž�����С��
 * @version   V1.0.0
 * @date      2018-02-28
 * @brief     �ļ��ڰ�����sp3485�Ĳ����������������͡����ա����豸���ر��豸�Ȳ���
 * @History
 * Date           Author    version    		Notes
 * 2018-02-28     ZSY       V1.0.0          first version.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "sp3485.h"
#include "rtthread.h"
#include "delay.h"
		
/* Private macro Definition --------------------------------------------------*/   
/* �궨��busy��Ӳ���ӿ� */
#define SP3485_BUSY_PIN             GPIO_Pin_1
#define SP3485_BUSY_PORT            GPIOA
#define SP3485_BUSY_GPIO_RCC      	RCC_AHB1Periph_GPIOA

#define SP3485_CS_PIN               GPIO_Pin_0
#define SP3485_CS_PORT              GPIOA
#define SP3485_CS_GPIO_RCC          RCC_AHB1Periph_GPIOA

/* ����sp3485��busy�ߵĶ��빦��	*/
#define	SP3485_BUSY_READ            GPIO_ReadInputDataBit(SP3485_BUSY_PORT, SP3485_BUSY_PIN)

#define	SP3485_CS_H                 GPIO_SetBits(SP3485_CS_PORT, SP3485_CS_PIN)
#define	SP3485_CS_L                 GPIO_ResetBits(SP3485_CS_PORT, SP3485_CS_PIN)

/* End private macro Definition ----------------------------------------------*/
/* global variable Declaration -----------------------------------------------*/
rt_device_t sp3485_device;
static rt_mailbox_t sp3485_rev_mb = RT_NULL;  
static rt_mutex_t  sp3485_lock_mutex = RT_NULL;

/* User function Declaration --------------------------------------------------*/

/* User functions -------------------------------------------------------------*/

/**
 * @func	sp3485_input
 * @brief 	sp3485�豸�������ݻص�����
 * @param	dev �豸����
 * @param	size ���ݴ�С
 * @note  	
 * @retval	�����ɹ�����RT_EOK
 */
static rt_err_t sp3485_input(rt_device_t dev, rt_size_t size)
{
    /* ͨ�����䷢�ͽ��յ������ݵĴ�С */
    rt_mb_send(sp3485_rev_mb, size);
    
    return RT_EOK;
}

/**
 * @func	sp3485_busy_gpio_config
 * @brief 	��ʼ��æ�ź���
 * @note  	
 * @retval	��
 */
static void sp3485_busy_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��GPIOA��ʱ�� */
    RCC_AHB1PeriphClockCmd(SP3485_BUSY_GPIO_RCC | SP3485_CS_GPIO_RCC, ENABLE);
		
    /**	config PIN Status
     * alternate function IO output
     * push-Pull output
     * speed is 100MHz
     * pull output
    */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;	
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
    
    /* config iic scl line */
    GPIO_InitStructure.GPIO_Pin = SP3485_BUSY_PIN;  		//PC5  
    GPIO_Init(SP3485_BUSY_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SP3485_CS_PIN;  		//PC5  
    GPIO_Init(SP3485_CS_PORT, &GPIO_InitStructure);
}

/**
 * @func	sp3485_init
 * @brief 	��ʼ��sp3485
 * @note  	
 * @retval	�����ɹ�����RT_EOK
 */
rt_err_t sp3485_init(void)
{
    /* ��ʼ������ */
    sp3485_busy_gpio_config();
    
    SP3485_CS_L;
    
    return RT_EOK;
}

/**
 * @func	sp3485_send_dta
 * @brief 	��sp3485�豸��������
 * @param	* dta ����ָ��
 * @param	size ���ݴ�С
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t sp3485_send_data(const uint8_t * data_buf, rt_size_t size)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (!(sp3485_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("sp3485 serial device no openning.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    /* ��ס�豸 */
    rt_mutex_take(sp3485_lock_mutex, RT_WAITING_FOREVER);
    
    SP3485_CS_H;
    /* ���豸д������ */
    do
    {
        len = rt_device_write(sp3485_device, 0, data_buf, size);
        
        timeout++;
    } while (len == 0 && timeout < 500);
    
    delay_ms(5);
    
    SP3485_CS_L;
    /* �ͷ��豸 */
    rt_mutex_release(sp3485_lock_mutex);
    
    /* �ж��Ƿ�ʱ����ʱ�������ʧ�� */
    if (timeout >= 500)
    {
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

rt_size_t sp3485_get_rev_status(void)
{
    rt_size_t data_size;
    rt_err_t rt_err;
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (!(sp3485_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("sp3485 serial device no openning.\r\n");
#endif
        return False;
    }
    
    if (sp3485_rev_mb == RT_NULL)
    {
        return False;
    }
    
    /* �ȴ�����,��ȡ���ݵĴ�С */
    rt_err = rt_mb_recv(sp3485_rev_mb, (rt_size_t *)&data_size, RT_WAITING_FOREVER);
    
    if (rt_err != RT_EOK)
    {
        return False;
    }
    
    return data_size;
}

/**
 * @func	sp3485_rev_dta
 * @brief 	��sp3485�Ͻ�������
 * @param	* dta �������ݻ�����ָ��
 * @param	* size �������ݴ�С�Ļ���ָ��
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t sp3485_rev_data(uint8_t * data_buf, rt_size_t size)
{
    __IO rt_size_t result = 0;

    RT_ASSERT(data_buf);
    
    /* ��ס�豸 */
    rt_mutex_take(sp3485_lock_mutex, RT_WAITING_FOREVER);
    
    /* ��ȡ���� */
    result = rt_device_read(sp3485_device, 0, data_buf, size);
    
    /* �ͷ��豸 */
    rt_mutex_release(sp3485_lock_mutex);
    
    if (result != size)
    {
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 * @func	sp3485_open
 * @brief 	��sp3485�豸�Թ�����
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t sp3485_open(void)
{
    rt_err_t rt_err;
    
    /* ���ݶ�����Ѱ���豸 */
    sp3485_device = rt_device_find(SP3485_USING_UART);
    
    /* �Ҳ����豸�򷵻ش������ */
    if (sp3485_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;
    }
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (sp3485_device->open_flag & RT_DEVICE_OFLAG_OPEN)
    {
#ifdef RT_DEBUG
        rt_kprintf("Device is open.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �豸�ص����� */
    rt_err = rt_device_set_rx_indicate(sp3485_device, sp3485_input);
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
            rt_kprintf("set %s rx indicate erroe, %d\r\n", SP3485_USING_UART, rt_err);
#endif
            return -RT_ERROR;
    }
    
    /* ���豸 */
    rt_err = rt_device_open(sp3485_device, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
    
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
        rt_kprintf("open %s device error.%d\r\n", SP3485_USING_UART, rt_err);
#endif
        return -RT_ERROR;
    }
    
    /* ���������ź��� */
    sp3485_lock_mutex = rt_mutex_create("sp3485_mutex", RT_IPC_FLAG_FIFO);
    
    if (sp3485_lock_mutex == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("Can`t create sp3485_mutex.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    sp3485_rev_mb = rt_mb_create("sp3485 rev mb", 32, RT_IPC_FLAG_FIFO);    
    if (sp3485_rev_mb == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("create sp3485 rev mb fault!\r\n");
#endif
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 * @func	sp3485_close
 * @brief 	�ر�sp3485�豸
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t sp3485_close(void)
{
    rt_err_t rt_err;
    
    rt_device_t device;
    
    /* ���ݶ�����Ѱ���豸 */
    device = rt_device_find(SP3485_USING_UART);
    
    if (device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (!(device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("Device no open.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �ر��豸 */
    rt_err = rt_device_close(device);
    
    /* ɾ�������Ļ����ź��� */
    rt_err = rt_mutex_delete(sp3485_lock_mutex);
    
    rt_err = rt_mb_delete(sp3485_rev_mb);
    
    return RT_EOK;
}

/**
 * @func	sp3485_busy
 * @brief 	sp3485æ���
 * @note  	
 * @retval	�豸æ����-SP3485_BUSY�����򷵻�SP3485_NO_BUSY
 */
rt_err_t sp3485_busy(void)
{
    if (SP3485_BUSY_READ == SET)
    {
        return -SP3485_BUSY;
    }
    else
    {
        return SP3485_NO_BUSY;
    }
}

/**
 * @func	sp3485_control
 * @brief 	����sp3485�豸
 * @note  	
 * @retval	��
 */
rt_err_t sp3485_control(void)
{
    return RT_EOK;
}

