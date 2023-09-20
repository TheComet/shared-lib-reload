#include "shared.h"

#if defined(_WIN32)
#   include <WinSock2.h>
#   include <ws2ipdef.h>
#   include <ws2tcpip.h>
#   define close closesocket
#else
#   include <unistd.h>
#   include <netdb.h>
#   include <sys/socket.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

struct some_obj* create_some_obj(const char* name, int x, int y, int z)
{
    struct some_obj* o = malloc(sizeof *o);
    o->name = malloc(strlen(name) + 1);
    strcpy(o->name, name);
    o->x = x;
    o->y = y;
    o->z = z;
    return o;
}
void destroy_some_obj(struct some_obj* o)
{
    free(o->name);
    free(o);
}

void net_init(void)
{
#if defined(_WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}
void net_deinit(void)
{
#if defined(_WIN32)
    WSACleanup();
#endif
}
int net_host(const char* bind_address, const char* port)
{
    struct addrinfo hints;
    struct addrinfo* candidates;
    struct addrinfo* p;
    int ret;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;        /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */
    ret = getaddrinfo(bind_address, port, &hints, &candidates);

    for (p = candidates; p; p = p->ai_next)
    {
        int yes = 1;
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        printf("bind(): %s\n", strerror(errno));
        close(sockfd);
    }
    freeaddrinfo(candidates);
    if (p == NULL)
        return -1;

    if (listen(sockfd, 1) == -1)
    {
        printf("listen(): %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    return sockfd;
}
int net_connect(const char* ip, const char* port)
{
    struct addrinfo hints;
    struct addrinfo* candidates;
    struct addrinfo* p;
    int ret;
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;        /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */
    ret = getaddrinfo(ip, port, &hints, &candidates);

    for (p = candidates; p; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        printf("connect(): %s\n", strerror(errno));
        close(sockfd);
    }
    freeaddrinfo(candidates);
    if (p == NULL)
        return -1;
    return sockfd;
}
void net_close(int sockfd)
{
    close(sockfd);
}
void net_send_msg(int sockfd, const char* msg)
{
    send(sockfd, msg, strlen(msg), 0);
}
int net_accept(int sockfd)
{
    struct sockaddr_storage client_addr;
    socklen_t sin_size = sizeof(client_addr);
    int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &sin_size);
    if (client_sockfd == -1)
        printf("accept(): %s\n", strerror(errno));
    return client_sockfd;
}
void net_recv_msg(int sockfd)
{
    char buf[64];
    int len = recv(sockfd, buf, 63, 0);
    if (len > 0)
    {
        buf[len] = '\0';
        printf("Received: %s\n", buf);
    }
}
