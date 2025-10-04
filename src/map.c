#include <malloc.h>
#include <wchar.h> // developing time
#include "bfind.h"
#pragma pack(4)
struct map_entity{
    int   key;
    const void *value;
};
#pragma pack(8)

void memmv(char *base,long idx,long end,int shift)
{
    for(int ix=end-1;ix>=idx;ix--){
        base[ix+shift]=base[ix];
    }
}

struct _MAP{
    int capcity;
    int cnt;
    char pairs[];
};
int map_cmp(const int *l,const int *r){
    if(*l<*r) return -1;
    if(*l> *r) return 1;
    return 0;
}
int map_lt(const int *l,const int *r){
    return *l < *r;
}
struct _MAP* map_new(int cap)
{
    struct _MAP *map=malloc(sizeof(struct _MAP)+cap * 12);
    map->capcity=cap;
    map->cnt=0;
    return map;
}

int map_set(struct _MAP *map,int key,const void *obj)
{
    if(map->cnt==0){
        struct map_entity *ptr=(struct map_entity *)&map->pairs[0];
        ptr->key=key;
        ptr->value=obj;
        map->cnt=1;
        return 0;
    }
    if(map->cnt==1){
        struct map_entity *ptr=(struct map_entity *)&map->pairs[0];
        if(key==ptr->key){
            ptr->value=obj;
            return 0;
        }
        if(key<ptr->key){
            struct map_entity *ptr1=ptr+1;
            ptr1->key=ptr->key;
            ptr1->value=ptr->value;
        } else {
            ptr ++;
        }
        ptr->key=key;
        ptr->value=obj;
        map->cnt=2;
        return 0;
    }
    long idx=bfind(map->pairs,&key,12,map->cnt,(LT)map_lt);
    struct map_entity *ptr=(struct map_entity *)&map->pairs[0];
    ptr += idx;
    if(idx==map->cnt){
        // 新key，而且位置应该是最后面的
        if(idx==map->capcity){
            return -1;
        }
        ptr->key=key;
        ptr->value=obj;
        map->cnt++;
        return 0;
    }
    if(ptr->key==key){
        ptr->value=obj;
    } else {
        if(map->cnt==map->capcity){
            // 容器已经满了
            return -1;
        }
        //memcpy(&map->pairs[(idx+1)*12],&map->pairs[idx*12],(map->cnt-idx)*12);
        memmv(&map->pairs[0],idx*12,map->cnt*12,12);
        ptr->key=key;
        ptr->value=obj;
        map->cnt++;
    }
    return 0;
}
int map_get(struct _MAP *map,int key,const void **ref)
{
    if(map->cnt==0){
        return -1;
    }
    long idx=bfind(map->pairs,&key,12,map->cnt,(LT)map_lt);
    if(idx==map->cnt){
        return -1;
    }
    struct map_entity *ptr=(struct map_entity *)&map->pairs[idx*12];
    if(ptr->key!=key){
        return -1;
    }
    *ref=ptr->value;
    return 0;
}
void map_clear(struct _MAP *map)
{
    map->cnt=0;
}

void map_print(struct _MAP *map)
{
    struct map_entity *ptr=(struct map_entity *)&map->pairs[0];
    for(int ix=0;ix<map->cnt;ix++){
        wprintf(L"key:%d,%ls\n",ptr[ix].key,(const wchar_t*)ptr[ix].value);
    }
}