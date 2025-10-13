#ifndef FIFO_H_JJDE333J
#define FIFO_H_JJDE333J
typedef void * FIFO;
FIFO fifo_new(int);
#define fifo_free(x) free(x)

int fifo_push(FIFO,void*);
int fifo_pop(FIFO,void**);
#endif
