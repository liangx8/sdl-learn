#include <stdio.h>
void print_rect(int v[])
{
    printf("x:%d,y:%d,w:%d,h:%d\n",v[0],v[1],v[2],v[3]);
}
void print_array(unsigned short v[],int size)
{
    for(int ix=0;ix<size;ix++){
        printf("%d, ",v[ix]);
    }
    printf("\n");
}
void print_square(char *data)
{
    printf("====================");
    for(int ix=0;ix<64;ix++){
        if((ix % 8) == 0){
            printf("\n");
        }
        if(*(data+ix)){
            printf("*");
        } else {
            printf(".");
        }
    }
    printf("\n====================\n");
}
