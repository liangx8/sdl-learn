#ifndef SAMPLE_CLASS_H
#define SAMPLE_CLASS_H
#define OP_ADD '+'
#define OP_SUBTRACT '-'
typedef struct _class_data{
    int operand[2];
    char op;
} CLASS_DATA;

void populate_random_class1(CLASS_DATA *data);

#endif // SAMPLE_CLASS_H