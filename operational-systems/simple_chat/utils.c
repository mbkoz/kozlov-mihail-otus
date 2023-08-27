#include <utils.h>

enum {
    USER_TABLE_SIZE = 100
};

void ST_init(SimpleTable* const self);
void ST_deinit(SimpleTable* const self);

/* таблица для регистрации пользователя по номеру сокета */
typedef struct sSimpleTable {
    void* table_[USER_TABLE_SIZE];
} SimpleTable;


SimpleTable* ST_create() {
    SimpleTable* ret = (SimpleTable*)malloc(sizeof(SimpleTable));
    if(ret == NULL)
        error("can't allocate sinple table");

    ST_init(ret);
    return ret;
}

void ST_init(SimpleTable* const self) {
    assert(self != NULL);
    for(size_t i = 0; i < USER_TABLE_SIZE; ++i)
        self->table_[i] = NULL;
}

void ST_destroy(SimpleTable* const self) {
    assert(self != NULL);
    ST_deinit(self);
    free(self);
}

void ST_deinit(SimpleTable* const self) {
    assert(self != NULL);
    (void)self;
}

bool ST_addElem(SimpleTable* const self, void* elem, size_t idx) {
    assert(self != NULL);
    assert(elem != NULL);
    
    if(idx >= USER_TABLE_SIZE)
        return false;

    self->table_[idx] = elem;
    return true;
}

void* ST_removeElem(SimpleTable* const self,  size_t idx) {
    assert(self != NULL);

    void* ret = NULL;
    if(idx < USER_TABLE_SIZE) {
        ret = self->table_[idx];
        self->table_[idx] = NULL;
    }

    return ret;
}

size_t ST_getMaxIndex(SimpleTable* const self) {
    (void)self;
    return USER_TABLE_SIZE;
}

Iterator Iterator_init(SimpleTable* const st) {
    assert(st != NULL);

    size_t i = 0;
    for(; i < USER_TABLE_SIZE; ++i)
        if(st->table_[i] != NULL)
            break;

    i %= USER_TABLE_SIZE;

    return (Iterator){
        .cur_ = i,
        .st_ = st
    };
}

void* Iterator_getElementAndAdvance(Iterator* const iter) {
    assert(iter != NULL);

    void* ret = iter->cur_ >= USER_TABLE_SIZE ? NULL : iter->st_->table_[iter->cur_];

    for(; iter->cur_ < USER_TABLE_SIZE; ++iter->cur_)
        if(iter->st_->table_[iter->cur_] != NULL)
            break;

    return ret;
}


typedef struct sSharedMsg {
    char msg_[SHARED_MSG_LEN];
    size_t ptrCnt_;
} SharedMsg;

SharedMsg* SM_create() {
    SharedMsg* ret = (SharedMsg*)malloc(sizeof(SharedMsg));

    if(ret == NULL)
        error("can't allocate shared message");

    ret->msg_[0] = '\0';
    ret->ptrCnt_ = 1;

    return ret;
}

void SM_destroy(SharedMsg* const self) {
    assert(self);
    
    if(--(self->ptrCnt_) == 0) {
        free(self);
    }
}

void SM_share(SharedMsg* const self) {
    ++(self->ptrCnt_);
}

char* SM_getRawPtr(SharedMsg* const self) {
    return self->msg_;
}

size_t SM_getRawDataLen(SharedMsg* const self) {
    return SHARED_MSG_LEN;
}