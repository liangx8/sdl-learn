#ifndef APP_ERR_H
#define APP_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

void app_err_push(const char *file, int line, const char *fmt, ...);
void err_stack_print(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_ERR_H */
