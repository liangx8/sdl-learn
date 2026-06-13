#include <stdlib.h>
#include "sample_class.h"

void populate_random_class1(CLASS_DATA *data) {
    if (!data) {
        return;
    }
    int num=rand();
    data->operand[0] =  (num % 9)+1;
    num=num /10;
    data->operand[1] = (num % 9)+1;
    data->op = OP_ADD;
    data->answer = -1;
}