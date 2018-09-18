/**
 ******************************************************************************
 * @file      queue.c
 * @author    门禁开发小组
 * @version   V1.0.1
 * @date      2018-02-28
 * @brief     文件内包含着队列的操作方法，需要rt-thread系统的支持
 * @History
 * Date           Author    version    		Notes
 * 2017-11-01     ZSY       V1.0.0          first version.
 * 2018-02-28     ZSY       V1.0.1          将队列嵌入到系统当中
 */
	
/* Includes ------------------------------------------------------------------*/
#include "queue.h"

/* global variable Declaration -----------------------------------------------*/

/* User function Declaration -------------------------------------------------*/


/* User functions ------------------------------------------------------------*/
/**
 * @func    queue_init
 * @brief   队列初始化
 * @param   * pq 队列对象
 * @note    
 * @retval  操作成功返回RT_EOK
 */
rt_err_t queue_init(queue_t * pq)
{
    rt_ubase_t  level;
    
    /* disable interrupt */
    level = rt_hw_interrupt_disable();
    
    /* 队列指向空 */
	pq->front = pq->rear = RT_NULL;
    
    /* 数目为零 */
	pq->count = 0;
    
    /* enable interrupt */
    rt_hw_interrupt_enable(level);
    
    return RT_EOK;
}

/**
 * @func    queue_is_full
 * @brief   判断队列是否为满
 * @param   * pq 队列对象
 * @note    
 * @retval  队列满则返回true，否则返回false
 */
rt_err_t queue_is_full(const queue_t * pq)
{
	return pq->count == MAXQUEUE;
}

/**
 * @func    queue_is_empty
 * @brief   判断队列是否为空
 * @param   * pq 队列对象
 * @note    
 * @retval  队列空则返回true，不为空则返回false
 */
rt_err_t queue_is_empty(const queue_t * pq)
{
	return pq->count == 0;
}

/**
 * @func    queue_item_count
 * @brief   计算队列的元素个数
 * @param   * pq 队列对象
 * @note    
 * @retval  返回节点数量
 */
rt_uint16_t queue_item_count(const queue_t * pq)
{
	return pq->count;
}

/**
 * @func    queue_copy_to_node
 * @brief   复制队列的元素的内容
 * @param   * pn 队列节点对象
 * @param   * qitem 队列节点对象的内容
 * @note    
 * @retval  无
 */
rt_inline void queue_copy_to_node(queue_node_t * pn, void * qitem)
{
	pn->qitem = qitem;
}

/**
 * @func    enqueue
 * @brief   向队列的末端添加一个元素
 * @param   * pq 队列对象
 * @param   * qitem 队列节点对象的内容
 * @note    
 * @retval  错误返回-RT_ERROR，成功返回RT_EOR
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
    
    /* 申请一块内存 */
	pnew = (queue_node_t *)rt_malloc(sizeof(queue_node_t));
	if (pnew == RT_NULL)
	{
        /* enable interrupt */
        rt_hw_interrupt_enable(level);
		return -RT_ERROR;
	}
    
    /* 复制数据 */
	queue_copy_to_node(pnew, qitem);
	pnew->next = RT_NULL;
    
    /* 安全判断 */
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
 * @brief   复制队列的元素的内容
 * @param   * pn 队列节点对象
 * @param   ** pitem 队列节点对象的内容
 * @note    
 * @retval  无
 */
rt_inline void queue_copy_to_item(queue_node_t * pn, void ** pitem)
{
	*pitem = pn->qitem;
}

/**
 * @func    dequeue
 * @brief   出队，提取当前队列的第一个元素
 * @param   * pq 队列对象
 * @note    
 * @retval  错误返回 RT_NULL，成功则返回节点指针
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
