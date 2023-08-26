/**
 * Библиотека для регистрации и хранения списка зарегестрированных пользователей (и данных профиля)
*/

#include <stdint.h>
#include <stdio.h>

/**
 * план на первую половину
 * V 1. дописать базовю функциональность (без времени регистрации)
 * V 2. написать скрипт, который бы создавал файл, проверял наличие записей в нем и вносил новые запии в него
 * так же проверял наличие уже существующих 
 * V 3. провести проверку с уже существующим файлом
 * V 4. провести проверку в valgrind на утечку
 * 5. доработать запись времени у регистрируемых пользователей
 * 6. создать makefile для сборка библиотеки в подкаталоге (+ посмотреть как делать make)
 * 7. в makefile сделать вариант с тестированием и отладкой
 * в makefile надо удалить предыдущую версию файла и создать новую
*/

typedef struct sUserList UserList;

typedef enum eUL_Status {
    UL_STATUS_SUCCESS = 0,
    UL_STATUS_ERROR_UNKNOWN_LOGIN = 1,
    UL_STATUS_ERROR_INVALID_PASSWORD = 2,
    UL_STATUS_ERROR_OUT_OF_LENGTH = 3,
    UL_STATUS_ERROR_USER_ALREADY_EXIST = 4,
    UL_STATUS_ERROR_INCORRECT_SYMBOLS = 5
} UL_Status;


/**
 * данные профиля
*/
typedef struct sUL_ProfileData {
    const char* login_;   // пока только логин, который будет использоваться как имя
} UL_ProfileData;

/**
 * @brief конструктор списка пользователей
 * @param file - имя файла со списком пользователей
*/
UserList* UL_create(const char* const fileName);

/**
 * @brief деструктор
*/
void UL_destroy(UserList* const self);

/**
 * @brief получение максимальной длины имени пользователй
*/
size_t UL_getMaxLoginLength(void);

/**
 * @brief получение максимальной длины пароля
*/
size_t UL_getMaxPasswordLength(void);

/**
 * @brief функция для получения данных профиля по логину и паролю
 * @return NULL если нет пользователя или неверно введен пароль
*/
UL_ProfileData UL_getProfileData(const UserList* const self, const char* const login, const char* const password);

/**
 * @brief функция для создания профиля
*/
void UL_setProfileData(UserList* const self, const char* const login, const char* const password);

/**
 * @brief функция для получения ошибки последней операции
*/
UL_Status UL_getStatus(void);
