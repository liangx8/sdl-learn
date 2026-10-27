# 自动执行

### __attribute__((cleanup(cleanup_func)))
使用实例
```
void auto_cleanup(void *ptr)
{
    printf("自动清理函数被调用");
}
int main()
{
    __attribute__ ((cleanup(auto_cleanup))) int a=10;
}
```

### __attribute__ ((constructor/destructor))