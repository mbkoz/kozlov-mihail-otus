#define _XOPEN_SOURCE 500 

#include "utils.h"

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>
#include <ulist.h>
#include <user.h>

/**
 * выход из сервера по SIGINT (ctrl + C)
 * в перехватчике сигнала надо закрыть все сокеты
 * при возникновении непредвиденной ошибки (AGAIN)
 * можно самостоятельно запускать сигнал
 * при подключении к серверку можно сделать так, чтобы клиент запрашивал некоторые метаданные сервера
 * например максимальную длину имени пользователя и пароля
 * В регистрации может быть отказано если будет пробита таблица
*/

//#if CLOCKS_PER_SEC > 1000000
//    #error "ololo"
//#endif


// вероятно стоит сделать объект Global Init который будет читать командную строку и DEF значение
// и отвечать за инициализацию всего остального
enum {
    DEFAULT_BLOCK_TIME_MS = 100,
    DEFAULT_THREAD_POOL_SIZE = 2,
    DEFAULT_PORT = 30000,
    MAX_USR_NAME_LEN = 128,
    DEFAULT_MAX_USR_NUMBER = 10,        // максимальное количество пользователей в чате
    DEFAULT_CYCLE_DURATION_MS = 1000,    // продолжительность цикла опроса дескрипторов в мс
    CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000,
    CLOCKS_PER_US = CLOCKS_PER_MS / 1000,
    DEFAULT_CYCLE_DURATION_CLOCKS = DEFAULT_CYCLE_DURATION_MS * CLOCKS_PER_MS,
};



static inline void setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        error("can't set nonblocking");
}

static void signalrq(int num) {
    printf("\tsignal num %d catched!\n", num);
    exit(EXIT_SUCCESS);
}




typedef struct sUser User;




int main(int argc, char* argv[]) {
    // загрузка кеша имен пользователей и паролей
    // создание сокета сервера
    // bind сокета к порту
    // создание объекта epoll и привязка сокета к прослушиванию
    // циклический контроль за событиями над просушиваемыми клиентами и сокетом
    // если событие от сокета, то подключаем нового клиента
    // если событие чтения/записи от клиента, то ставим в очередь функцию чтения/записи
    // если событие зарытия клиента, то ставим в очередь событие деициниализации

    struct sigaction action = {0};
    action.sa_handler = signalrq;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);



    int listner = socket(PF_INET, SOCK_STREAM, 0);
    if(listner == -1)
        error("can't open socket");

    struct sockaddr_in name = {0};
    name.sin_family = PF_INET;
    name.sin_port = (in_port_t)htons(DEFAULT_PORT);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    if(setsockopt(listner, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == -1)
        error("can't set reuse socket");
    
    if(bind(listner, (struct sockaddr*)&name, sizeof(name)) == -1)
        error("can't bind socket");

    if(listen(listner, 10) == -1)
        error("can't listen socket");


    struct epoll_event events[DEFAULT_MAX_USR_NUMBER], evTemp;

    int epollfd = epoll_create1(0);
    if(epollfd == -1)
        error("can't create epoll");

    evTemp = (struct epoll_event){.events = EPOLLIN, .data = {.fd = listner}};
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listner, &evTemp) == -1)
        error("can't add listner to epoll");

    // временно для теста
    struct sockaddr_storage clientAddr = {0};
    unsigned int clientAddrSize = sizeof(clientAddr);
    int client;

    UserList* ul = UL_create("ul_file");
    UL_destroy(ul);

    while(true) {
        clock_t epollStart = clock();
        time_t currTime = time(NULL);
        struct tm lt = *localtime(&currTime);

        int nfds = epoll_wait(epollfd, events, DEFAULT_MAX_USR_NUMBER, DEFAULT_CYCLE_DURATION_MS);
        if(nfds == -1)
            error("epoll wait error");

        clock_t epollFinish = clock();

        for(size_t i = 0; i < nfds; ++i) {
            if(events[i].data.fd == listner){
                // получаем новое подключение
                client = accept(listner, (struct sockaddr*)&clientAddr, &clientAddrSize);
                if(client == -1)
                    error("can't connect with client");
                // делаем его неблокирующим
                setnonblocking(client);
                // зарешестрируем epoll для ожидания доступности чтения или записи
                evTemp = (struct epoll_event){
                    .events = EPOLLIN | EPOLLOUT | EPOLLET,
                    .data = {
                        .fd = client
                    }
                };
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, client, &evTemp) == -1)
                    error("can't add client to epoll");
                printf("%02d:%02d:%02d\tadd client to listen\n", lt.tm_hour, lt.tm_min, lt.tm_sec);
            }
            else {
                // генерим очередь задач для пула потоков (пока просто объектаня модель)
                printf("fs: %d\n", events[i].data.fd);
                if(events[i].events & EPOLLHUP) {
                    // если fd отключился, то удалить fd из наблюдения и закрыть подключение
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &evTemp);
                    close(events[i].data.fd);
                    printf("%02d:%02d:%02d\tfile is closed(EPOLLHUP)\n", lt.tm_hour, lt.tm_min, lt.tm_sec);

                } else {
                    if(events[i].events & EPOLLIN) {
                        char temp[1024];
                        int cnt;

                        printf("%02d:%02d:%02d\thave data for reading\n", lt.tm_hour, lt.tm_min, lt.tm_sec);
                        while((cnt = recv(events[i].data.fd, temp, 1023, 0)) > 0) {
                            if(cnt == 1023 && temp[1022] != '\n')
                                temp[1024] = '\0';
                            else
                                temp[cnt - 1] = '\0';
                            puts(temp);
                        }
                        if(cnt == 0)
                            puts("file is closed(read proc)");
                        if(cnt == -1)
                            puts(strerror(errno));
                    }
                    if(events[i].events & EPOLLOUT) {
                        // отправим сообщение в ответ
                        // надо провести 2 опыта. Отправить однократное сообщение, проверить
                        // будет ли потом срабатываться EPOLLET и если не будет, то отправить много данных, чтобы заблокировать вывод
                        printf("%02d:%02d:%02d\thave data to write\n", lt.tm_hour, lt.tm_min, lt.tm_sec);
                        char temp[1024];
                        sprintf(temp, "%02d:%02d:%02d\thello from server!\n", lt.tm_hour, lt.tm_min, lt.tm_sec);
                        if(send(events[i].data.fd, temp, strlen(temp), 0) == -1)
                            puts(strerror(errno));
                        else
                            puts("writing correct");

                        puts(strerror(errno));
                    }
                }
            }
        }

        clock_t procFinish = clock();

        printf("epollStart: %ld, epollFinish: %ld, procFinish: %ld\n", epollStart, epollFinish, procFinish);

        clock_t durClocks = procFinish - epollStart;
        printf("duration clocks: %ld\n", durClocks);

        if(durClocks < DEFAULT_CYCLE_DURATION_CLOCKS)
            usleep((DEFAULT_CYCLE_DURATION_CLOCKS - durClocks) / CLOCKS_PER_US);

        printf("*********************************%02d:%02d:%02d*********************************\n", lt.tm_hour, lt.tm_min, lt.tm_sec);
        fflush(stdout);
    }

    return 0;
}

