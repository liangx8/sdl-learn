#pragma once
/**
 * @brief first in first out container
 */
typedef struct _QUEUE QUEUE;

QUEUE *queue_new(int size);
/**
 * @brief 加一个对象
 * @return return 0 or -1 if QUEUE is full
 */
int queue_push_back(QUEUE *q,void *obj1,void *obj2);
/**
 * @brief 抽取一个对象
 * @return return 0 or -1 if QUEUE is empty
 */
int queue_pop_front(QUEUE *,void **,void **);