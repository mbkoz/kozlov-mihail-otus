# Основные тезисы

1. Решение состоит из 2 частей: сервера и клиента
2. сервер отвечает за прием сообщений от клиентов и пересылку их другим клиентам.
3. сервер имеет пул потоков, который асинхронно обрабатывает сообщения от клиентов (рассылает их по адресатам)
4. сервер должен иметь следующие опции командной строки: порт, количество потоков в пуле, максимальное количество клиентов, интервал ожидания
событий в epoll, количество выгружаемых событий
5. сервер должен поддерживать регистрацию и авторизацию пользователей. Для хранения имен ипаролей будет использоваться зашифрованный файл.
Данные будут шифроваться при помощи so библиотеки. В текущей реализации сервера будет использоваться шифрованием XOR'ом.
6. сервер должен писать лог. Лог будет работать в общем цикле и отражать работу epoll.
7. При закрытии сервера, сервер закроет все подключения к клиентам.

8. клиент - приложение с tui, которое позволяет регистрироваться/авторизовываться на сервере и 
писать сообщения всем или конкретному человеку/группе
9. клиент в качестве параметров командной строки должен принимать: номер порта сервера, адрес сервера, имя и пароль пользователя.
10. В tui клиента должна быть строка ввода сообщения, и меню действий (авторизация, выход). А после авторизации должен дополнительно появляется
список участников чата с возможностью выбора, кому отправить сообщение.


# реализация сервера

1. основной примитив сервера User - класс, который ассоциирован с приемом и передачей сообщений
2. User имеет набор состояний: регистрация/авторизация, полуечние списка пользователей, обмен сообщениями, закрыт
3. Для неблокирующего приема сообщений будет использоваться функционал epoll, который получает набор файловых дескрипторов
и тип события, который для них произошел.
4. После создания epoll объекта в конце работы надо вызвать close
5. 

# protocol

**типы сообщений протокола:** 
1. регистрация/аутентификация (может быть сделана 1 сообщением)
2. получение данных о списке участников (в формате отсноистельных изменений, обязательно с подтверждением)
3. пересылка сообщений



Вопросы по epoll: 
формат результата (какие данные epoll выдает, когда происходит событие):
структура fd_event - номер дескриптора с событием, OR событий и данные, которые были записаны в момент регистрации в ctl

одновременное наблюдение за вводом и выводом от одного файлового дескриптора
возможно

определение закрытия файловог дескриптора за которым наблюдает epoll
определяется автомаетически. EPOLLHUP - флаг, который всегда мониторится и отвечает за контроллинг открытого сокета

Изменение конфигурации наблюдения за файловым дескриптором на лету (во время работы wait)
потокобезопасно, возможно из другого потока

Удаление файлового дескриптора из наблюдения на лету
потокобезопасно, возможно из другого потока

нужна библиотека с контейнерами (хэш таблица, множество)