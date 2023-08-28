#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <assert.h>
#include <fcntl.h>

enum {
    SHARED_MSG_LEN = 1024,
};


static inline void error(const char* const str) {
    fprintf(stderr, "error has occuried: %s, error message: %s\n", strerror(errno), str);
    exit(EXIT_FAILURE);
}

static inline void setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        error("can't set nonblocking");
}

/* таблица для регистрации пользователя по номеру сокета */
typedef struct sSimpleTable SimpleTable;


SimpleTable* ST_create();
void ST_destroy(SimpleTable* const self);
bool ST_addElem(SimpleTable* const self, void* elem, size_t idx);
void* ST_removeElem(SimpleTable* const self,  size_t idx);
size_t ST_getMaxIndex(SimpleTable* const self);

typedef struct sIterator {
    SimpleTable* st_;
    size_t cur_;
} Iterator;

/**
 * @brief вернет итератор на первый не NULL элемент
*/
Iterator Iterator_init(SimpleTable* const st);
void* Iterator_getElementAndAdvance(Iterator* const iter);


typedef struct sSharedMsg SharedMsg;

SharedMsg* SM_create();
void SM_destroy(SharedMsg* const self);
void SM_share(SharedMsg* const self);

char* SM_getRawPtr(SharedMsg* const self);
size_t SM_getRawDataLen(SharedMsg* const self);
