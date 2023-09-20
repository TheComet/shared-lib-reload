#include "shared.h"

#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#else
#   include <dlfcn.h>
#endif

#include <stdio.h>

static create_some_obj_fn create_some_obj;
static destroy_some_obj_fn destroy_some_obj;

static net_init_fn net_init;
static net_deinit_fn net_deinit;
static net_host_fn net_host;
static net_connect_fn net_connect;
static net_close_fn net_close;
static net_send_msg_fn net_send_msg;
static net_accept_fn net_accept;
static net_recv_msg_fn net_recv_msg;

static void* lib_handle;

#if defined(_WIN32)
#define dlsym GetProcAddress
#define dlclose FreeLibrary
#endif

static void close_shared_lib(void)
{
    if (lib_handle)
        dlclose(lib_handle);
    lib_handle = NULL;
}
static void reload_shared_lib(void)
{
    close_shared_lib();

#if defined(_WIN32)
    lib_handle = LoadLibrary("Debug\\shared.dll");
#else
    lib_handle = dlopen("./shared.so", RTLD_LAZY);
#endif
    *(void**)&create_some_obj  = dlsym(lib_handle, "create_some_obj");
    *(void**)&destroy_some_obj = dlsym(lib_handle, "destroy_some_obj");
    *(void**)&net_init         = dlsym(lib_handle, "net_init");
    *(void**)&net_deinit       = dlsym(lib_handle, "net_deinit");
    *(void**)&net_host         = dlsym(lib_handle, "net_host");
    *(void**)&net_connect      = dlsym(lib_handle, "net_connect");
    *(void**)&net_close        = dlsym(lib_handle, "net_close");
    *(void**)&net_send_msg     = dlsym(lib_handle, "net_send_msg");
    *(void**)&net_accept       = dlsym(lib_handle, "net_accept");
    *(void**)&net_recv_msg     = dlsym(lib_handle, "net_recv_msg");
}

int main(int argc, char** argv)
{
    reload_shared_lib();
    net_init();

    struct some_obj* o = create_some_obj("object name", 5, 6, 7);

    int server_sockfd = net_host("0.0.0.0", "5000");
    int client_sockfd = net_connect("127.0.0.1", "5000");
    int server_client_sockfd = net_accept(server_sockfd);

    printf("o->name: \"%s\"\n", o->name);
    net_send_msg(client_sockfd, "Hello, sending client -> server!");
    net_recv_msg(server_client_sockfd);
    net_send_msg(server_client_sockfd, "Hello, sending server -> client!");
    net_recv_msg(client_sockfd);

    puts("Reloading shared lib...");
    reload_shared_lib();

    printf("o->name: \"%s\"\n", o->name);
    net_send_msg(client_sockfd, "Hello, sending client -> server!");
    net_recv_msg(server_client_sockfd);
    net_send_msg(server_client_sockfd, "Hello, sending server -> client!");
    net_recv_msg(client_sockfd);

    net_close(server_client_sockfd);
    net_close(client_sockfd);
    net_close(server_sockfd);
    destroy_some_obj(o);

    net_deinit();

    return 0;
}

