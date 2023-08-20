#include "user_list.h"
#include "sglib.h"

#include <threads.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

enum {
    STR_LEN = 64,
    FILE_NAME_LEN = 256,
};

thread_local static UL_Status status = UL_STATUS_SUCCESS; 

static inline void error(const char* const str) {
    assert(str);
    fprintf(stderr, "error has occuried: %s, error message: %s\n", strerror(errno), str);
    exit(EXIT_FAILURE);
}

/**
 * @brief структура с данными профиля (пока только логин и пароль)
*/
typedef struct sListNode {
    char login_[STR_LEN];
    char password_[STR_LEN];
    struct sListNode* next_;
} ListNode;


static ListNode* createListNode(const char* const login, const char* const password);
static void printListNode(const ListNode* const ln, FILE* file);
static void destroyListNode(ListNode* const self);

static int cmpListNodes(const ListNode* const lhs, const ListNode* const rhs);

SGLIB_DEFINE_SORTED_LIST_PROTOTYPES(ListNode, cmpListNodes, next_)
SGLIB_DEFINE_SORTED_LIST_FUNCTIONS(ListNode, cmpListNodes, next_)

typedef struct sUserList {
    char filename_[FILE_NAME_LEN];
    ListNode* start_;
} UserList;

static void UL_init(UserList* const self, const char* const fileName);
static void UL_deinit(UserList* const self);


UL_Status UL_getStatus(void) {
    return status;
}

size_t UL_getMaxLoginLength(void) {
    return STR_LEN - 1;
}

size_t UL_getMaxPasswordLength(void) {
    return STR_LEN - 1;
}

UserList* UL_create(const char* const fileName) {
    assert(fileName);

    UserList* ul = (UserList*)malloc(sizeof(UserList));
    if(ul == NULL)
        error("can't allocate user list");

    UL_init(ul, fileName);

    return ul;
}

void UL_destroy(UserList* const self) {
    assert(self);
    UL_deinit(self);
    free(self);
}


static ListNode* createListNode(const char* const login, const char* const password) {
    assert(login != NULL);
    assert(password != NULL);

    ListNode* ln = (ListNode*)malloc(sizeof(ListNode));
    if(ln == NULL)
        error("can't allocate list node");

    strncpy(ln->login_, login, STR_LEN - 1);
    ln->login_[STR_LEN - 1] = '\0';

    strncpy(ln->password_, password, STR_LEN - 1);
    ln->password_[STR_LEN - 1] = '\0';

    ln->next_ = NULL;

    return ln;
}

static void destroyListNode(ListNode* const ln) {
    assert(ln != NULL);

    free(ln);
}

static int cmpListNodes(const ListNode* const lhs, const ListNode* const rhs) {
    assert(lhs != NULL);
    assert(rhs != NULL);

    return strcmp(lhs->login_, rhs->login_);
}

static void printListNode(const ListNode* const ln, FILE* file) {
    assert(ln != NULL);
    assert(file != NULL);

    fprintf(file, "%s %s\n", ln->login_, ln->password_);
}

static void UL_init(UserList* const self, const char* const fileName) {
    strncpy(self->filename_, fileName, FILE_NAME_LEN - 1);
    self->filename_[FILE_NAME_LEN - 1] = '\0';
    // открываем файл на чтение и читаем записи создавая новые узлы
    FILE* file = fopen(self->filename_, "r");
    if(file == NULL)
        error("can't open file with users list");
    
    // create "" "" to make sglib happy =)
    self->start_ = NULL;
    char login[STR_LEN], password[STR_LEN];

    while(fscanf(file, "%s %s", login, password) == 2)
        sglib_ListNode_add(&self->start_, createListNode(login, password));

    // закрываем файл
    if(!feof(file))
        error("error while readine user list");

    fclose(file);
}

static void UL_deinit(UserList* const self) {
    // открываем файл на запись
    FILE* file = fopen(self->filename_, "w");
    if(file == NULL)
        error("can't open file with users list");

    // сохраняем узлы и удаляем
    ListNode* curln = self->start_;
    while(curln != NULL) {
        ListNode* prevln = curln;
        curln = curln->next_;
        printListNode(prevln, file);
        destroyListNode(prevln);
    }

    // закрываем файл
    fclose(file);
}

UL_ProfileData UL_getProfileData(const UserList* const self, const char* const login, const char* const password) {
    UL_ProfileData ret = {.login_ = NULL};

    if(strlen(login) >= STR_LEN || strlen(password) >= STR_LEN){
        status = UL_STATUS_ERROR_OUT_OF_LENGTH;
        return ret;
    }

    ListNode* target = createListNode(login, password);
    ListNode* res = sglib_ListNode_find_member(self->start_, target);
    
    if(res == NULL) {
        status = UL_STATUS_ERROR_UNKNOWN_LOGIN;
    }
    else if(strcmp(target->password_, res->password_)) {
        status = UL_STATUS_ERROR_INVALID_PASSWORD;
    }
    else {
        status = UL_STATUS_SUCCESS;
        ret.login_ = res->login_;
    }

    destroyListNode(target);

    return ret;
}

void UL_setProfileData(UserList* const self, const char* const login, const char* const password) {
    if(strlen(login) >= STR_LEN || strlen(password) >= STR_LEN){
        status = UL_STATUS_ERROR_OUT_OF_LENGTH;
        return;
    }
    else if(strchr(login, ' ') || strchr(password, ' ')) {
        status = UL_STATUS_ERROR_INCORRECT_SYMBOLS;
        return;
    }

    ListNode* ln = createListNode(login, password);
    
    if(sglib_ListNode_find_member(self->start_, ln)) {
        status = UL_STATUS_ERROR_USER_ALREADY_EXIST;
        destroyListNode(ln);
    }
    else {
        sglib_ListNode_add(&self->start_, ln);
        status = UL_STATUS_SUCCESS;
    }
}



static inline void testUserV(const UserList* const ul, const char* const usr, const char* const password) {
    UL_ProfileData pd = UL_getProfileData(ul, usr, password);

    printf("get profile data for login \'%s\' password \'%s\'\n", usr, password);

    if(UL_getStatus() == UL_STATUS_SUCCESS) {
        printf("find login \'%s\' with password \'%s\'\n", usr, password);
    }
    else if(UL_getStatus() == UL_STATUS_ERROR_UNKNOWN_LOGIN) {
        printf("login \'%s\' doesn't exist\n", usr);
    }
    else if(UL_getStatus() == UL_STATUS_ERROR_INVALID_PASSWORD) {
        printf("incorrect password \'%s\' for login \'%s\'\n", password, usr);
    }
    putchar('\n');
}

static inline void addUserV(UserList* const ul, const char* const usr, const char* const password) {
    printf("add profile with login \'%s\' password \'%s\'\n", usr, password);

    UL_setProfileData(ul, usr, password);
    if(UL_getStatus() == UL_STATUS_SUCCESS) {
        printf("login \'%s\' with password \'%s\' added\n", usr, password);
    }
    else if (UL_getStatus() == UL_STATUS_ERROR_USER_ALREADY_EXIST) {
        printf("login \'%s\' already exist\n", usr);
    }
    else if (UL_getStatus() == UL_STATUS_ERROR_INCORRECT_SYMBOLS) {
        printf("login \'%s\' or password \'%s\' have incorrect symbols\n", usr, password);
    }

    putchar('\n');
}

int main(int argc, char* argv[]) {
    UserList* ul = UL_create("user_list_file");

    printf("maximum login length: %lu\nmaximum password length: %lu\n\n", UL_getMaxLoginLength(), UL_getMaxPasswordLength());

    testUserV(ul, "misha", "1111");
    addUserV(ul, "misha", "1111");

    testUserV(ul, "sasha", "1111");
    testUserV(ul, "misha", "2222");
    testUserV(ul, "misha", "1111");

    addUserV(ul, "alesha", "2222");
    addUserV(ul, "sasha", "3333");
    addUserV(ul, "C()()L_HACKEr", "4444");

    addUserV(ul, "misha", "1111");
    addUserV(ul, "misha", "2222");

    testUserV(ul, "C()()L_HACKEr", "4444");
    testUserV(ul, "C()()L_HACKEr", "42");
    testUserV(ul, "elele", "4444");

    addUserV(ul, "misha kozlov", "2222");

    UL_destroy(ul);

    return 0;
}


