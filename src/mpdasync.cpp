#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cassert>

extern "C" {
    #include <fcntl.h>
    #include <sys/epoll.h>
}

#include <mpd/client.h>
#include <mpd/parser.h>
#include <mpd/async.h>
#include <mpd/connection.h>

#include "mpdnotify.h"

constexpr int MAX_EVENTS {10};

constexpr int TIMEOUT {1};

bool wait_with_epoll(int fd) {
    // create a new epoll, no CLOEXEC
    int epollfd = epoll_create1(0);

    epoll_event ev {}, events[MAX_EVENTS];

    ev.data.fd = fd;
    ev.events = EPOLLIN|EPOLLRDHUP|EPOLLERR;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        return false;

    int nfs = epoll_wait(epollfd, events, MAX_EVENTS, TIMEOUT);
    if (nfs == -1) return false;

    std::cout << "got " << nfs << " events" << std::endl;

    for (int i {0}; i < nfs; ++i) {
        if (events[i].events & EPOLLRDHUP) {
            std::cout << "server closed connection" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    return true;
}

void handle_mpd_error(enum mpd_server_error errcode) {
    std::cout << "got error code " << errcode << std::endl;
}

void run_parser(mpd_async *async) {
    enum mpd_async_event recvd {};
    recvd = mpd_async_events(async);

    struct mpd_parser *parser = mpd_parser_new();

    char *line {nullptr};

    const char *name {nullptr};
    const char *value {nullptr};

    while (recvd & MPD_ASYNC_EVENT_READ) {
        line = mpd_async_recv_line(async);

        switch (mpd_parser_feed(parser, line)) {
            case MPD_PARSER_MALFORMED:
            std::cout << "parser malformed" << std::endl;
            break;

            case MPD_PARSER_ERROR:
            handle_mpd_error(mpd_parser_get_server_error(parser));
            break;

            case MPD_PARSER_SUCCESS:
            std::cout << "parser success" << std::endl;
            mpd_parser_free(parser);
            return;
            break;

            case MPD_PARSER_PAIR:
            name = mpd_parser_get_name(parser);
            value = mpd_parser_get_value(parser);

            std::cout << "name: " << name << ", value: " << value << std::endl;
            break;
        }
        recvd = mpd_async_events(async);
    }

    mpd_parser_free(parser);
}


int main(int argc, char*argv[]) {
    mpd_connection *conn = mpd_connection_new(nullptr, 0, 0);
    assert(conn != nullptr);
    mpd_async *async = mpd_connection_get_async(conn);

    int fd = mpd_async_get_fd(async);
    if (async == nullptr) {
        perror("could not get async: ");
        std::exit(EXIT_FAILURE);
    }
    if (!mpd_async_set_keepalive(async, true)) {
        perror("setsockopt failed: ");
        std::exit(EXIT_FAILURE);
    }

    bool success = mpd_async_send_command(async, "currentsong", nullptr);
    if (!mpd_async_io(async, MPD_ASYNC_EVENT_WRITE) || !success) {
        perror("failed to send command");
        std::exit(EXIT_FAILURE);
    }

    std::cout << "waiting for epoll" << std::endl;

    if (!wait_with_epoll(fd)) {
        perror("epoll failed: ");
        std::exit(EXIT_FAILURE);
    } /* this blocks until epoll returns something */

    if (!mpd_async_io(async, MPD_ASYNC_EVENT_READ)) {
        const char *err = mpd_async_get_error_message(async);
        std::cout << "got error message " << err << std::endl;
        perror("io operations failed");
        std::exit(EXIT_FAILURE);
    }

    std::cout << "recvd events" << std::endl;

    run_parser(async);
    mpd_connection_free(conn);
    return EXIT_SUCCESS;
    
}