/**
 ******************************************************************************
 * @Copyright       (C) 2017 - 2018 guet-sctc-hardwarepart Team
 * @filename        queue.h
 * @author          门禁开发小组
 * @version         V1.0.1
 * @date            2018-02-28
 * @Description     queue头文件，在此文件内实现队列对外开放的API接口。
 * @Others
 * @History
 * Date           Author    version    		Notes
 * 2017-11-04     ZSY       V1.0.0          first version.
 * 2018-02-28     ZSY       V1.0.1          将队列嵌入到系统当中.
 * @verbatim  
 */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _Queue_H_
#define _Queue_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "rtthread.h"

/* Public macro Definition ---------------------------------------------------*/
#define MAXQUEUE 80
/* End public macro Definition -----------------------------------------------*/

/* UserCode start ------------------------------------------------------------*/
typedef struct _queue_node
{
	void * qitem;
	struct _queue_node * next;
}queue_node_t;

typedef struct _queue
{
	queue_node_t * front;
	queue_node_t * rear;
	int count;
}queue_t;

/* Member method APIs --------------------------------------------------------*/
/*初始化队列*/
rt_err_t queue_init(queue_t * pq);
/*判断队列是否为满*/
rt_err_t queue_is_full(const queue_t * pq);
/*判断队列是否为空*/
rt_err_t queue_is_empty(const queue_t * pq);
/*获取队列的元素的个数*/
rt_uint16_t queue_item_count(const queue_t * pq);
/*进队*/
rt_err_t enqueue(queue_t * pq, void * qitem);
/*出队*/
void * dequeue(queue_t * pq);
/* End Member Method APIs ---------------------------------------------------*/
/* UserCode end -------------------------------------------------------------*/
	
#endif
