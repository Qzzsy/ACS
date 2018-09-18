#include "fingerprint.h"
#include "delay.h"

rt_device_t fingerprint_device;

static rt_mailbox_t fingerprint_rev_mb = RT_NULL;  
static rt_mutex_t  fingerprint_lock_mutex = RT_NULL;

/**
 * @func	fingerprint_input
 * @brief 	fingerprint设备接收数据回调函数
 * @param	dev 设备对象
 * @param	size 数据大小
 * @note  	
 * @retval	操作成功返回RT_EOK
 */
static rt_err_t fingerprint_input(rt_device_t dev, rt_size_t size)
{
    /* 通过邮箱发送接收到的数据的大小 */
    rt_mb_send(fingerprint_rev_mb, size);
    
    return RT_EOK;
}

/**
 * @func	fingerprint_send_data
 * @brief 	向fingerprint设备发送数据
 * @param	* data 数据指针
 * @param	size 数据大小
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t fingerprint_send_data(const char * data, rt_size_t size)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    
    /* 判断设备是否已经打开 */
    if (!(fingerprint_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("fingerprint serial device hasn`t openning.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    /* 锁住设备 */
    rt_mutex_take(fingerprint_lock_mutex, RT_WAITING_FOREVER);
    
    /* 向设备写入数据 */
    do
    {
        len = rt_device_write(fingerprint_device, 0, data, size);
        
        timeout++;
    } while (len == 0 && timeout < 500);
    
    delay_ms(5);
    
    /* 释放设备 */
    rt_mutex_release(fingerprint_lock_mutex);
    
    /* 判断是否超时，超时则代表发送失败 */
    if (timeout >= 500)
    {
        return -RT_ERROR;
    }
    
    return RT_EOK;
}


rt_size_t fingerprint_get_rev_status(void)
{
    rt_size_t data_size;
    rt_err_t rt_err;
    
    /* 判断设备是否已经打开 */
    if (!(fingerprint_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("fingerprint serial device no openning.\r\n");
#endif
        return False;
    }
    
    if (fingerprint_rev_mb == RT_NULL)
    {
        return False;
    }
    
    /* 等待数据,获取数据的大小 */
    rt_err = rt_mb_recv(fingerprint_rev_mb, (rt_size_t *)&data_size, RT_WAITING_FOREVER);
    
    if (rt_err != RT_EOK)
    {
        return False;
    }
    
    return data_size;
}

/**
 * @func	fingerprint_rev_dta
 * @brief 	从fingerprint上接收数据
 * @param	* data 接收数据缓存区指针
 * @param	* size 接收数据大小的缓存指针
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t fingerprint_rev_data(uint8_t * data_buf, rt_size_t size)
{
    __IO rt_size_t result = 0;

    RT_ASSERT(data_buf);
    
    /* 锁住设备 */
    rt_mutex_take(fingerprint_lock_mutex, RT_WAITING_FOREVER);

    /* 获取数据 */
    result = rt_device_read(fingerprint_device, 0, data_buf, size);
    
    /* 释放设备 */
    rt_mutex_release(fingerprint_lock_mutex);
    
    if (result != size)
    {
#ifdef RT_DEBUG
        rt_kprintf("read data fault.\r\n");
#endif
        
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 * @func	fingerprint_open
 * @brief 	打开fingerprint设备以供访问
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t fingerprint_open(void)
{
    rt_err_t rt_err;
    
    /* 根据对象名寻找设备 */
    fingerprint_device = rt_device_find(FINGERPRINT_USING_UART);
    
    /* 找不到设备则返回错误代码 */
    if (fingerprint_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;
    }
    
    /* 判断设备是否已经打开 */
    if (fingerprint_device->open_flag & RT_DEVICE_OFLAG_OPEN)
    {
#ifdef RT_DEBUG
        rt_kprintf("Device is open.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 设备回调函数 */
    rt_err = rt_device_set_rx_indicate(fingerprint_device, fingerprint_input);
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
            rt_kprintf("set %s rx indicate erroe, %d\r\n", FINGERPRINT_USING_UART, rt_err);
#endif
            return -RT_ERROR;
    }
    
    /* 打开设备 */
    rt_err = rt_device_open(fingerprint_device, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
    
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
        rt_kprintf("open %s device error.%d\r\n", FINGERPRINT_USING_UART, rt_err);
#endif
        return -RT_ERROR;
    }
    
    /* 创建互斥信号量 */
    fingerprint_lock_mutex = rt_mutex_create("fingerprint_mutex", RT_IPC_FLAG_FIFO);
    
    if (fingerprint_lock_mutex == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("Can`t create fingerprint_mutex.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    fingerprint_rev_mb = rt_mb_create("fingerprint rev mb", 32, RT_IPC_FLAG_FIFO);    
    if (fingerprint_rev_mb == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("create fingerprint rev mb fault!\r\n");
#endif
        return -RT_ERROR;
    }
    
    return RT_EOK;
}

/**
 * @func	fingerprint_close
 * @brief 	关闭fingerprint设备
 * @note  	
 * @retval	操作成功返回RT_EOK，否则返回-RT_ERROR
 */
rt_err_t fingerprint_close(void)
{
    rt_err_t rt_err;
    
    rt_device_t device;
    
    /* 根据对象名寻找设备 */
    device = rt_device_find(FINGERPRINT_USING_UART);
    
    if (device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 判断设备是否已经打开 */
    if (!(device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("Device no open.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* 关闭设备 */
    rt_err = rt_device_close(device);
    
    /* 删除创建的互斥信号量 */
    rt_err = rt_mutex_delete(fingerprint_lock_mutex);
    
    rt_err = rt_mb_delete(fingerprint_rev_mb);
    
    return RT_EOK;
}

/**
 * @func	fingerprint_control
 * @brief 	控制fingerprint设备
 * @note  	
 * @retval	无
 */
rt_err_t fingerprint_control(void)
{
    return RT_EOK;
}
