#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define HTTP_THREAD_STACK_SIZE 4096
#define HTTP_THREAD_PRIORITY 7

void http_server_thread(void);

#endif // HTTP_SERVER_H
