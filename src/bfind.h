#pragma once
typedef int (*LT)(const void *,const void *);
long bfind(const void *base,const void *key,int size,long num,LT lt);