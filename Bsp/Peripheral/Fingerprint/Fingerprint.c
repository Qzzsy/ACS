#include "fingerprint.h"
#include "delay.h"

rt_device_t fingerprint_device;

static rt_mailbox_t fingerprint_rev_mb = RT_NULL;  
static rt_mutex_t  fingerprint_lock_mutex = RT_NULL;

/**
 * @func	fingerprint_input
 * @brief 	fingerprint�豸�������ݻص�����
 * @param	dev �豸����
 * @param	size ���ݴ�С
 * @note  	
 * @retval	�����ɹ�����RT_EOK
 */
static rt_err_t fingerprint_input(rt_device_t dev, rt_size_t size)
{
    /* ͨ�����䷢�ͽ��յ������ݵĴ�С */
    rt_mb_send(fingerprint_rev_mb, size);
    
    return RT_EOK;
}

/**
 * @func	fingerprint_send_data
 * @brief 	��fingerprint�豸��������
 * @param	* data ����ָ��
 * @param	size ���ݴ�С
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t fingerprint_send_data(const char * data, rt_size_t size)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (!(fingerprint_device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("fingerprint serial device hasn`t openning.\r\n");
#endif
        return -RT_ERROR;        
    }
    
    /* ��ס�豸 */
    rt_mutex_take(fingerprint_lock_mutex, RT_WAITING_FOREVER);
    
    /* ���豸д������ */
    do
    {
        len = rt_device_write(fingerprint_device, 0, data, size);
        
        timeout++;
    } while (len == 0 && timeout < 500);
    
    delay_ms(5);
    
    /* �ͷ��豸 */
    rt_mutex_release(fingerprint_lock_mutex);
    
    /* �ж��Ƿ�ʱ����ʱ�������ʧ�� */
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
    
    /* �ж��豸�Ƿ��Ѿ��� */
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
    
    /* �ȴ�����,��ȡ���ݵĴ�С */
    rt_err = rt_mb_recv(fingerprint_rev_mb, (rt_size_t *)&data_size, RT_WAITING_FOREVER);
    
    if (rt_err != RT_EOK)
    {
        return False;
    }
    
    return data_size;
}

/**
 * @func	fingerprint_rev_dta
 * @brief 	��fingerprint�Ͻ�������
 * @param	* data �������ݻ�����ָ��
 * @param	* size �������ݴ�С�Ļ���ָ��
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t fingerprint_rev_data(uint8_t * data_buf, rt_size_t size)
{
    __IO rt_size_t result = 0;

    RT_ASSERT(data_buf);
    
    /* ��ס�豸 */
    rt_mutex_take(fingerprint_lock_mutex, RT_WAITING_FOREVER);

    /* ��ȡ���� */
    result = rt_device_read(fingerprint_device, 0, data_buf, size);
    
    /* �ͷ��豸 */
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
 * @brief 	��fingerprint�豸�Թ�����
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t fingerprint_open(void)
{
    rt_err_t rt_err;
    
    /* ���ݶ�����Ѱ���豸 */
    fingerprint_device = rt_device_find(FINGERPRINT_USING_UART);
    
    /* �Ҳ����豸�򷵻ش������ */
    if (fingerprint_device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;
    }
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (fingerprint_device->open_flag & RT_DEVICE_OFLAG_OPEN)
    {
#ifdef RT_DEBUG
        rt_kprintf("Device is open.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �豸�ص����� */
    rt_err = rt_device_set_rx_indicate(fingerprint_device, fingerprint_input);
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
            rt_kprintf("set %s rx indicate erroe, %d\r\n", FINGERPRINT_USING_UART, rt_err);
#endif
            return -RT_ERROR;
    }
    
    /* ���豸 */
    rt_err = rt_device_open(fingerprint_device, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
    
    if (rt_err != RT_EOK)
    {
#ifdef RT_DEBUG
        rt_kprintf("open %s device error.%d\r\n", FINGERPRINT_USING_UART, rt_err);
#endif
        return -RT_ERROR;
    }
    
    /* ���������ź��� */
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
 * @brief 	�ر�fingerprint�豸
 * @note  	
 * @retval	�����ɹ�����RT_EOK�����򷵻�-RT_ERROR
 */
rt_err_t fingerprint_close(void)
{
    rt_err_t rt_err;
    
    rt_device_t device;
    
    /* ���ݶ�����Ѱ���豸 */
    device = rt_device_find(FINGERPRINT_USING_UART);
    
    if (device == RT_NULL)
    {
#ifdef RT_DEBUG
        rt_kprintf("can`t find %s device.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �ж��豸�Ƿ��Ѿ��� */
    if (!(device->open_flag & RT_DEVICE_OFLAG_OPEN))
    {
#ifdef RT_DEBUG
        rt_kprintf("Device no open.\r\n", FINGERPRINT_USING_UART);
#endif
        return -RT_ERROR;        
    }
    
    /* �ر��豸 */
    rt_err = rt_device_close(device);
    
    /* ɾ�������Ļ����ź��� */
    rt_err = rt_mutex_delete(fingerprint_lock_mutex);
    
    rt_err = rt_mb_delete(fingerprint_rev_mb);
    
    return RT_EOK;
}

/**
 * @func	fingerprint_control
 * @brief 	����fingerprint�豸
 * @note  	
 * @retval	��
 */
rt_err_t fingerprint_control(void)
{
    return RT_EOK;
}
