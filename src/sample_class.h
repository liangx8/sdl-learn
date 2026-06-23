#ifndef SAMPLE_CLASS_H
#define SAMPLE_CLASS_H
#include <stdint.h>
#define OP_ADD '+'
#define OP_SUBTRACT '-'
typedef struct _class_data{
    uint64_t num[81];
    int idx;
    int answer;
    int seq;
} CLASS_DATA;

void populate_random_class1(CLASS_DATA *data);
int class_check(CLASS_DATA *data);

#endif // SAMPLE_CLASS_H