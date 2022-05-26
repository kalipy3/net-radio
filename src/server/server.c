/*
 * server.c
 * Copyright (C) 2022 2022-05-09 16:39 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>



#include "../include/proto.h"
#include "server_conf.h"
#include "server.h"

/*
 * -M   指定多播组
 * -P   指定接收端口
 * -F   前台运行
 * -D   指定媒体库位置
 * -I   指定网络设备
 * -H   显示帮助
 */

//全局变量即使在别的文件用不上,也尽量用extern声明在.h文件里
struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,\
    .mgroup = DEFAULT_MGROUP,\
        .media_dir = DEFAULT_MEDIADIR,\
        .runmode = RUN_DAEMON,\
        .ifname = DEFAULT_IF
}

static void printfhelp(void)
{
    printf("-M   指定多播组\n");
    printf("-P   指定接收端口\n");
    printf("-F   前台运行\n");
    printf("-D   指定媒体库位置\n");
    printf("-I   指定网络设备\n");
    printf("-H   显示帮助\n");
}

static void daemon_exit(int s)
{
    closelog();

    exit(0);
}

static int daemonize(void)
{
    int fd;
    pid_t pid;

    pid = fork();
    if (pid < 0)
    {
        //perror("fork()");
        syslog(LOG_ERR, "fork():%s", strerror(errno));
        return -1;
    }

    if (pid > 0) //parent
        exit(0);

    fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        //perror("open()");
        syslog(LOG_WARNING, "open():%s", strerror(errno));
        return -2;
    } else {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);

        if (fd > 2)
            close(fd);
    }

    setsid();

    chdir("/home/kalipy/gg");
    umask(0);

    return 0;
}

static socket_init(void)
{
    int serversd;
    struct ip_mreqn mreq;

    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serversd < 0)
    {
        syslog(LOG_ERR, "socket():%s", strerror(errno));
        exit(1);
    }

    inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex(server_conf.ifname);

    if (setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
    {
        syslog(LOG_ERR, "setsockopt(IP_MULTICAST_IF):%s", strerror(errno));
        exit(1);
    }

    //bind();
}

int main(int argc, char *argv[])
{
    int c;
    struct sigaction sa;
    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    openlog("netradio", LOG_PID | LOG_PERROR)

        /*命令行分析*/

        while (1)
        {
            c = getopt(argc, argv, "M:P:FD:I:H");
            if (c < 0)
                break;
            switch (c)
            {
                case 'M':
                    server_conf.mgroup = optarg;
                    break;
                case 'P':
                    server_conf.rcvport = optarg;
                    break;
                case 'F':
                    server_conf.runmode = RUN_FOREGROUND;
                    break;
                case 'D':
                    server_conf.media_dir = optarg;
                    break;
                case 'I':
                    server_conf.ifname = optarg;
                    break;
                case 'H':
                    printfhelp();
                    exit(0);
                    break;
                default:
                    abort();
                    break;
            }
        }

    /*守护进程的实现*/
    if (server_conf.runmode == RUN_DAEMON) {
        if (daemonize() != 0)//变成守护进程之后 1.接下来的程序是没有办法在终端输出的(因为守护进程脱离了控制终端),我们可以用系统log的方式记录输出 2.守护进程必然是意外被终止的,比如信号
            exit(1);
    }
    else if (server_conf.runmode == RUN_FOREGROUND) {
        //do nothing
    } else {
        syslog(LOG_ERR, "EINVAL server_conf.runmode.");
        exit(1);
    }

    /*SOCKET初始化*/
    socket_init();

    /*获取频道信息*/

    /*创建节目单线程*/

    /*创建频道线程*/

    while (1)
        pause();

}

