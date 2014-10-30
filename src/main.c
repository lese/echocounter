#include "test.h"

#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#define SERVER_PORT 3333

typedef struct {
    int num_derps;
    int num_accepts;
} Counter;

static void on_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address,
        int socklen, void *ctx);
static void on_accept_error(struct evconnlistener *listener, void *ctx);

static void on_buffer_read(struct bufferevent *bev, void *ctx);
static void on_buffer_event(struct bufferevent* bev, short events, void *ctx);

int main(int argc, char *argv[])
{
    struct event_base *ev_base = event_base_new();
    if (!ev_base) {
        perror("Could not open event base\n");
        return -1;
    }

    // Prevent program from closing when endpoint closes connection before our send finishes.
    signal(SIGPIPE, SIG_IGN);

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));

    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(SERVER_PORT);

    Counter counter;
    memset(&counter, 0, sizeof(counter));

    struct evconnlistener *listener = evconnlistener_new_bind(ev_base,
            on_accept, &counter, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1,
            (struct sockaddr*) &listen_addr, sizeof(listen_addr));
    if (!listener) {
        perror("Could not create evconnlistener\n");
        return -1;
    }

    evconnlistener_set_error_cb(listener, on_accept_error);
    event_base_dispatch(ev_base);

    puts("Closed listen\n");
    return 0;
}

static void on_accept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address,
        int socklen, void *ctx)
{
    Counter *counter = (Counter*) ctx;
    counter->num_accepts++;

    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, on_buffer_read, NULL, on_buffer_event, ctx);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    puts("Accepted connection");
}

static void on_accept_error(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);

    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got accept error %d (%s). Shutting down.\n", err,
            evutil_socket_error_to_string(err));

    evconnlistener_free(listener);
    event_base_loopexit(base, NULL);
}

static void on_buffer_read(struct bufferevent *bev, void *ctx)
{
    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);

    size_t length = evbuffer_get_length(input);
    char *data = malloc(length);
    evbuffer_remove(input, data, length);

    printf("Received data: %s\n", data);

    Counter *counter = (Counter*) ctx;
    if (strcmp(data, "derp\n") == 0) {
        counter->num_derps++;
    }

    free(data);

    char hello[30];
    copy_hello(hello, sizeof(hello));

    evbuffer_add_printf(output, "%s!\n > Accepts: %d\n > Derps: %d\n",
            hello, counter->num_accepts, counter->num_derps);
}

static void on_buffer_event(struct bufferevent* bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR) {
        perror("Error from bufferevent");
    }

    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        puts("Closing connection");
        bufferevent_free(bev);
    }
}

