/**
 ******************************************************************************
 * @file      queue.c
 * @author    �Ž�����С��
 * @version   V1.0.1
 * @date      2018-02-28
 * @brief     �ļ��ڰ����Ŷ��еĲ�����������Ҫrt-threadϵͳ��֧��
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0          first version.
 * 2018-02-28     ZSY       V1.0.1          ������Ƕ�뵽ϵͳ����
 */
	
/* Includes ------------------------------------------------------------------*/
#include "queue.h"

/* global variable Declaration -----------------------------------------------*/

/* User function Declaration -------------------------------------------------*/


/* User functions ------------------------------------------------------------*/
/**
 * @func    queue_init
 * @brief   ���г�ʼ��
 * @param   * pq ���ж���
 * @note    
 * @retval  �����ɹ�����RT_EOK
 */
rt_err_t queue_init(queue_t * pq)
{
    rt_ubase_t  level;
    
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    
    /* ����ָ��� */
	pq->front = pq->rear = RT_NULL;
    
    /* ��ĿΪ�� */
	pq->count = 0;
    
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
    
    return RT_EOK;
}

/**
 * @func    queue_is_full
 * @brief   �ж϶����Ƿ�Ϊ��
 * @param   * pq ���ж���
 * @note    
 * @retval  �������򷵻�true�����򷵻�false
 */
rt_err_t queue_is_full(const queue_t * pq)
{
	return pq->count == MAXQUEUE;
}

/**
 * @func    queue_is_empty
 * @brief   �ж϶����Ƿ�Ϊ��
 * @param   * pq ���ж���
 * @note    
 * @retval  ���п��򷵻�true����Ϊ���򷵻�false
 */
rt_err_t queue_is_empty(const queue_t * pq)
{
	return pq->count == 0;
}

/**
 * @func    queue_item_count
 * @brief   ������е�Ԫ�ظ���
 * @param   * pq ���ж���
 * @note    
 * @retval  ���ؽڵ�����
 */
rt_uint16_t queue_item_count(const queue_t * pq)
{
	return pq->count;
}

/**
 * @func    queue_copy_to_node
 * @brief   ���ƶ��е�Ԫ�ص�����
 * @param   * pn ���нڵ����
 * @param   * qitem ���нڵ���������
 * @note    
 * @retval  ��
 */
rt_inline void queue_copy_to_node(queue_node_t * pn, void * qitem)
{
	pn->qitem = qitem;
}

/**
 * @func    enqueue
 * @brief   ����е�ĩ�����һ��Ԫ��
 * @param   * pq ���ж���
 * @param   * qitem ���нڵ���������
 * @note    
 * @retval  ���󷵻�-RT_ERROR���ɹ�����RT_EOR
 */
rt_err_t enqueue(queue_t * pq, void * qitem)
{
    rt_ubase_t  level;
	queue_node_t * pnew;
    
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    
	if (queue_is_full(pq))
	{
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
		return -RT_ERROR;
	}
    
    /* ����һ���ڴ� */
	pnew = (queue_node_t *)rt_malloc(sizeof(queue_node_t));
	if (pnew == RT_NULL)
	{
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
		return -RT_ERROR;
	}
    
    /* �������� */
	queue_copy_to_node(pnew, qitem);
	pnew->next = RT_NULL;
    
    /* ��ȫ�ж� */
	if (queue_is_empty(pq))
	{
		pq->front = pnew;
	}
	else
	{
		pq->rear->next = pnew;
	}
    
	pq->rear = pnew;
	pq->count++;
	
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
    
	return RT_EOK;
}

/**
 * @func    queue_copy_to_item
 * @brief   ���ƶ��е�Ԫ�ص�����
 * @param   * pn ���нڵ����
 * @param   ** pitem ���нڵ���������
 * @note    
 * @retval  ��
 */
rt_inline void queue_copy_to_item(queue_node_t * pn, void ** pitem)
{
	*pitem = pn->qitem;
}

/**
 * @func    dequeue
 * @brief   ���ӣ���ȡ��ǰ���еĵ�һ��Ԫ��
 * @param   * pq ���ж���
 * @note    
 * @retval  ���󷵻� RT_NULL���ɹ��򷵻ؽڵ�ָ��
 */
void * dequeue(queue_t * pq)
{
    rt_ubase_t  level;
    
	queue_node_t * pt;
	void * pitem = NULL;
	
    level = rt_hw_interrupt_disable();
    
	if (queue_is_empty(pq))
	{
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
		return RT_NULL;
	}
    
	queue_copy_to_item(pq->front, &pitem);
    
	pt = pq->front;
	pq->front = pq->front->next;
    
	rt_free(pt);
    
	pt = RT_NULL;
	pq->count--;
	if (pq->count == 0)
	{
		pq->rear = RT_NULL;
	}
	
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
	return pitem;
}
