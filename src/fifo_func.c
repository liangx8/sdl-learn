#include <malloc.h>
#include <wchar.h>
struct _fifo{
    int pos1;
    int pos2;
    int total;
    int unused;
    void *obj[];
};

void *fifo_new(int size)
{
    struct _fifo *ptr=malloc(sizeof(struct _fifo)+sizeof(void *)*size);
    if(ptr==NULL){
        return NULL;
    }
    ptr->pos1=0;
    ptr->pos2=0;
    ptr->total=size;
    return ptr;
}
int fifo_push(struct _fifo *ptr,void *obj)
{
    int idx=ptr->pos2;
    if(idx>=ptr->total){
        // FIFO is full
        return -1;
    }
    ptr->obj[idx]=obj;
    idx++;
    if(idx>=ptr->total){
        idx=0;
    }
    if(idx==ptr->pos1){
        ptr->pos2=ptr->total;
    } else {
        ptr->pos2=idx;
    }
    return 0;
}
int fifo_pop(struct _fifo *ptr,void **ref)
{
    int idx=ptr->pos1;
    if(idx==ptr->pos2){
        // empty
        return -1;
    }
    *ref=ptr->obj[idx];
    if(ptr->pos2>=ptr->total){
        // FIFO full
        ptr->pos2=idx;
    }
    idx++;
    if(idx==ptr->total){
        idx=0;
    }
    ptr->pos1=idx;
    return 0;
}