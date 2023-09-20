#if defined(_WIN32) && defined(SHARED_BUILDING)
#   define SHARED_API __declspec(dllexport)
#else
#   define SHARED_API
#endif

struct some_obj
{
    char* name;
    int x, y, z;
};

#if defined(SHARED_BUILDING)

SHARED_API struct some_obj* create_some_obj(const char* name, int x, int y, int z);
SHARED_API void destroy_some_obj(struct some_obj* o);

SHARED_API void net_init(void);
SHARED_API void net_deinit(void);
SHARED_API int net_host(const char* bind_address, const char* port);
SHARED_API int net_connect(const char* ip, const char* port);
SHARED_API void net_close(int sockfd);
SHARED_API void net_send_msg(int sockfd, const char* msg);
SHARED_API int net_accept(int sockfd);
SHARED_API void net_recv_msg(int sockfd);

#else

typedef struct some_obj* (*create_some_obj_fn)(const char*, int, int, int);
typedef void (*destroy_some_obj_fn)(struct some_obj*);

typedef void (*net_init_fn)(void);
typedef void (*net_deinit_fn)(void);
typedef int (*net_host_fn)(const char*, const char*);
typedef int (*net_connect_fn)(const char*, const char*);
typedef void (*net_close_fn)(int);
typedef void (*net_send_msg_fn)(int, const char*);
typedef int (*net_accept_fn)(int);
typedef void (*net_recv_msg_fn)(int);

#endif

