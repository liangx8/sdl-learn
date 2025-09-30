typedef int (*LT)(const void *,const void *);
/**
 * @brief 在排序队列中查找,
 * @param base 排序队列
 * @param key 
 * @param size
 * @param num
 * @param lt
 */
long bfind(const void *base,const void *key,int size,long num,LT lt)
{
    long p1,p2;
    p1=0;
    p2=num;
    while(1){
        if(p1+1==p2){
            const void *ptr=base + p1 * size;
            if(lt(ptr,key)){
                return p2;
            }
            return p1;
        }
        long mid=(p1+p2)/2;
        const void *ptr=base + mid * size;
        if(lt(key,ptr)){
            p2=mid;
        } else {
            p1=mid;
        }
    }
}