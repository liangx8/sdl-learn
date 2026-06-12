#include <stdlib.h>
#include "sample_class.h"

void populate_random_class1(CLASS_DATA *data) {
    if (!data) {
        return;
    }
    int num=rand();
    data->operand[0] =  num % 10;
    num=num /10;
    data->operand[1] = num % 10;
    data->op = OP_ADD;
}