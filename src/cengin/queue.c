#include <malloc.h>
#include <stdio.h>
typedef struct {
    int size;
    int start,end;
    int padding[1];
    void *data[0];
} QUEUE;
QUEUE *queue_new(int size){
    const int offset = sizeof(QUEUE);
    const int us     = sizeof(void *);
    QUEUE *ptr       = malloc(offset + size * us * 2);
    if(ptr == NULL){
        return NULL;
    }
    ptr->start = 0;
    ptr->end   = 0;
    ptr->size  = size;
    return ptr;
}

int queue_push_back(QUEUE *q,void *obj1,void *obj2){
    if(q->end == __INT_MAX__){
        return -1;
    }
    q->data[q->end*2]=obj1;
    q->data[q->end*2+1]=obj2;
    int end=q->end+1;
    if(end==q->size) end=0;
    if(end==q->start){
        // is full
        q->end=__INT_MAX__;
    } else {
        q->end=end;
    }
    return 0;
}
int queue_pop_front(QUEUE *qu,void **ref_obj1,void **ref_obj2){
    int start=qu->start;
    if(start==qu->end){
        return -1;
    }
    int end=qu->end;
    if(end==__INT_MAX__){
        qu->end=start;
    }
    *ref_obj1=qu->data[start*2];
    *ref_obj2=qu->data[start*2+1];
    start ++;
    if(start == qu->size)qu->start=0;else qu->start=start;
    return 0;
}
