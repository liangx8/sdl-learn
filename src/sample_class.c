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
int class_check(CLASS_DATA *data)
{
    unsigned long num=data->num[data->idx];
    int opr2=num%9 + 1;
    num=num / 9;
    int opr1=num%9 +1;
    return opr2 + opr1 == data->answer;
}