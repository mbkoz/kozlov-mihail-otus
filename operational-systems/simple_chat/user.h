#pragma once

#include <stdlib.h>
#include <ulist.h>
#include <utils.h>


/**
 * используем основную структуру User как основную часть для приема собщений и отправки их по адресатам
 * работа с 
*/
typedef bool (*AcceptNextMsgFunctionT)(void* const self);
typedef bool (*SendNextMsgFunctionT)(void* const self);


typedef struct sUser {
    int fd_;                // дескриптор сокета пользователя
    int epfd_;
    UserList* ulist_;       // библиотека со списком пользователей
    char* const name_;      // имя пользователя
    SimpleTable* ut_;          // ссылка на хешь таблицу с другими User'ми
    // ссылка на очередь сообщений 
    // состояние приема сообщений (прием )
    AcceptNextMsgFunctionT acceptNextMsg_;
    // состояние передачи сообщений
    SendNextMsgFunctionT sendNextMsg_;
} User;


/**
 * стадии существования пользователя
 * 1. подключение
 * 2. получение реквизитов сервера: максимальная длина имени и пароля, максимальная длина сообщения
 * 3. регистрация/аутентификация
 * 4. прием/передача сообщения с временными метками сервера
 * 5. закрытие подключения
*/

User USER_create();
