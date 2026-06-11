#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"

struct app_err {
    struct app_err *next;
    const char *file;
    int line;
    char errmsg[0];
};

struct app_err *app_err_head = (void *)0;

void app_err_push(const char *file, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    struct app_err *err = (struct app_err *)malloc(sizeof(struct app_err) + 1024);
    if (!err) {
        fprintf(stderr, "app_err_push: allocation failed\n");
        return;
    }

    err->file = file;
    err->line = line;
    vsnprintf(err->errmsg, 1024, fmt, args);
    err->next = app_err_head;
    app_err_head = err;

    va_end(args);
}

void err_stack_print(void) {
    struct app_err *err = app_err_head;
    if (!err) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "err_stack_print: no errors stored");
        return;
    }

    while (err) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s:%d: %s", err->file, err->line, err->errmsg);
        err = err->next;
    }
}