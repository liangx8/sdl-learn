#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sample_class.h"

void populate_random_class1(CLASS_DATA *data) {
    if (!data) {
        return;
    }
    data->idx++;
    data->answer=-1;
    data->seq++;

    if(data->idx >80){
        SDL_Log("生成新练习");
        for (int i = 80; i > 0; --i) {
            int j = rand() % (i + 1);
            uint64_t tmp = data->num[i];
            data->num[i] = data->num[j];
            data->num[j] = tmp;
        }

        data->idx = 0;
    }
}
extern const char *strIcons;
void log_class(CLASS_DATA *data)
{
    unsigned long num=data->num[data->idx];
    int opr2=num%9 + 1;
    num=num / 9;
    int opr1=num%9 +1;
    const char *p=strIcons+3;
    if(opr1+opr2==data->answer){
        p=strIcons;
    }
    SDL_Log("%3d:%d + %d = %2d %s",data->seq,opr1,opr2,data->answer,p);
}
int class_check(CLASS_DATA *data)
{
    unsigned long num=data->num[data->idx];
    int opr2=num%9 + 1;
    num=num / 9;
    int opr1=num%9 +1;
    return opr2 + opr1 == data->answer;
}