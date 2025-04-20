#ifndef SERVER_H
#define SERVER_H

int http_server_init(int port);
void http_server_process(void);
void http_server_cleanup(void);

#endif 