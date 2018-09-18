/**
 ******************************************************************************
 * @file      sp3485.c
 * @author    门禁开发小组
 * @version   V1.0.0
 * @date      2018-02-28
 * @brief     文件内包含着sp3485的操作方法，包含发送、接收、打开设备、关闭设备等操作
 * @History
 * Date           Author    version    		Notes
 * 2018-02-28     ZSY       V1.0.0          first version.
 */
 
/* Includes ------------------------------------------------------------------*/
#include "sp3485.h"
#include "rtthread.h"
#include "delay.h"
		
/* Private macro Definition --------------------------------------------------*/   
/* 宏定义busy的硬件接口 */
#define SP3485_BUSY_PIN             GPIO_Pin_1
#define SP3485_BUSY_PORT            GPIOA
#define SP3485_BUSY_GPIO_RCC      	RCC_AHB1Periph_GPIOA

#define SP3485_CS_PIN               GPIO_Pin_0
#define SP3485_CS_PORT              GPIOA
#define SP3485_CS_GPIO_RCC          RCC_AHB1Periph_GPIOA

/* 设置sp3485的busy线的读入功能	*/
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
 * @brief 	sp3485设备接收数据回调函数
 * @param	dev 设备对象
 * @param	size 数据大小
 * @note  	
 * @retval	操作成功返回RT_EOK
 */
static rt_err_t sp3485_input(rt_device_t dev, rt_size_t size)
{
    /* 通过邮箱发送接收到的数据的大小 */
    rt_mb_send(sp3485_rev_mb, size);
    
    return RT_EOK;
}

/**
 * @func	sp3485_busy_gpio_config
 * @brief 	初始化忙信号线
 * @note  	
 * @retval	无
 */
static void sp3485_busy_gpio_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能GPIOA的时钟 */
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
 * @brief 	初始化sp3485
 * @note  	
 * @retval	操作成功返回RT_EOK
 */
rt_err_t sp3485_init(void)
{
    /* 初始化引脚 */
    sp3485_busy_gpio_config();
    
    SP3485_CS_L;
    
    return RT_EOK;
}

/**
 * @func	sp3485_send_dta
 * @brief 	向sp3485设备发送数据
 * @param	* dta 数据指针
 * @param	size 数据大小
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t sp3485_send_data(const uint8_t * data_buf, rt_size_t size)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    
    /* 判断设备是否已经打开 */
    if (!(sp3485_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("sp3485 serial device no openning.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    /* 锁住设备 */
    rt_mutex_take(sp3485_lock_mutex, RT_WAITING_FOREVER);
    
    SP3485_CS_H;
    /* 向设备写入数据 */
    do
    {
        len = rt_device_write(sp3485_device, 0, data_buf, size);
        
        timeout++;
    } while (len == 0 && timeout < 500);
    
    delay_ms(5);
    
    SP3485_CS_L;
    /* 释放设备 */
    rt_mutex_release(sp3485_lock_mutex);
    
    /* 判断是否超时，超时则代表发送失败 */
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
    
    /* 判断设备是否已经打开 */
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
    
    /* 等待数据,获取数据的大小 */
    rt_err = rt_mb_recv(sp3485_rev_mb, (rt_size_t *)&data_size, RT_WAITING_FOREVER);
    
    if (rt_err != RT_EOK)
    {
        return False;
    }
    
    return data_size;
}

/**
 * @func	sp3485_rev_dta
 * @brief 	从sp3485上接收数据
 * @param	* dta 接收数据缓存区指针
 * @param	* size 接收数据大小的缓存指针
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t sp3485_rev_data(uint8_t * data_buf, rt_size_t size)
{
    __IO rt_size_t result = 0;

    RT_ASSERT(data_buf);
    
    /* 锁住设备 */
    rt_mutex_take(sp3485_lock_mutex, RT_WAITING_FOREVER);
    
    /* 获取数据 */
    result = rt_device_read(sp3485_device, 0, data_buf, size);
    
    /* 释放设备 */
    rt_mutex_release(sp3485_lock_mutex);
    
    if (result != size)
    {
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 * @func	sp3485_open
 * @brief 	打开sp3485设备以供访问
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t sp3485_open(void)
{
    rt_err_t rt_err;
    
    /* 根据对象名寻找设备 */
    sp3485_device = rt_device_find(SP3485_USING_UART);
    
    /* 找不到设备则返回错误代码 */
    if (sp3485_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;
    }
    
    /* 判断设备是否已经打开 */
    if (sp3485_device->open_flag & RT_DEVICE_OFLAG_OPEN)
    {
#ifdef RT_DEBUG
        rt_kprintf("Device is open.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 设备回调函数 */
    rt_err = rt_device_set_rx_indicate(sp3485_device, sp3485_input);
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
            rt_kprintf("set %s rx indicate erroe, %d\r\n", SP3485_USING_UART, rt_err);
#endif
            return -RT_ERROR;
    }
    
    /* 打开设备 */
    rt_err = rt_device_open(sp3485_device, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
    
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
        rt_kprintf("open %s device error.%d\r\n", SP3485_USING_UART, rt_err);
#endif
        return -RT_ERROR;
    }
    
    /* 创建互斥信号量 */
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
 * @brief 	关闭sp3485设备
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t sp3485_close(void)
{
    rt_err_t rt_err;
    
    rt_device_t device;
    
    /* 根据对象名寻找设备 */
    device = rt_device_find(SP3485_USING_UART);
    
    if (device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 判断设备是否已经打开 */
    if (!(device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("Device no open.\r\n", SP3485_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 关闭设备 */
    rt_err = rt_device_close(device);
    
    /* 删除创建的互斥信号量 */
    rt_err = rt_mutex_delete(sp3485_lock_mutex);
    
    rt_err = rt_mb_delete(sp3485_rev_mb);
    
    return RT_EOK;
}

/**
 * @func	sp3485_busy
 * @brief 	sp3485忙检测
 * @note  	
 * @retval	设备忙返回-SP3485_BUSY，否则返回SP3485_NO_BUSY
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
 * @brief 	控制sp3485设备
 * @note  	
 * @retval	无
 */
rt_err_t sp3485_control(void)
{
    return RT_EOK;
}

