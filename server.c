#include <stdio.h>
#include "libuv/include/uv.h"
#include <unistd.h>
#include <stdlib.h>

#define RESPONSE                   \
    "HTTP/1.1 200 OK\r\n"          \
    "Content-Type: text/plain\r\n" \
    "Content-Length: 12\r\n"       \
    "\r\n"                         \
    "hello world\n"

void close_cb(uv_handle_t *handle)
{
}

void alloc_cb(uv_handle_t *handle,
              size_t suggested_size,
              uv_buf_t *buf)
{
    buf->len = suggested_size;
    buf->base = malloc(sizeof(char) * suggested_size);
}

void shutdown_cb(uv_shutdown_t *req, int status)
{
}

void write_cb(uv_write_t *req, int status)
{
}

void read_cb(uv_stream_t *client,
             ssize_t nread,
             const uv_buf_t *buf)
{
    printf("%zu: %s\n", nread, buf->base);
    uv_read_stop(client);
    uv_write_t req1;
    uv_buf_t buffer = uv_buf_init(RESPONSE, strlen(RESPONSE));
    uv_write(&req1, client, &buffer, 1, write_cb);

    uv_shutdown_t req;
    uv_shutdown(&req, client, shutdown_cb);
}

void accept_cb(uv_stream_t *server, int status)
{
    printf("accepted conn\n");
    uv_tcp_t client;
    uv_tcp_init(server->loop, &client);
    int err = uv_accept(server, (struct uv_stream_s *)&client);
    if (err < 0)
    {
        fprintf(stderr, "error happened while accepting connection\n");
        exit(1);
    }
    uv_read_start((struct uv_stream_s *)&client, alloc_cb, read_cb);
}

int main()
{
    uv_loop_t *loop = uv_default_loop();
    uv_loop_init(loop);
    uv_tcp_t handle;

    uv_tcp_init(loop, &handle);
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);

    uv_tcp_bind(&handle, (struct sockaddr *)&server_addr, 0);
    uv_listen((uv_stream_t *)&handle, 15, accept_cb);
    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}