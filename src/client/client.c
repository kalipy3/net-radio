#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <net/if.h>

#include "client.h"

//#include <proto.h>//在Makefile中指定了后，就可以用<>括起来
#include "../include/proto.h"

struct client_conf_st client_conf = {
    .rcvport = DEFAULT_RCVPORT,
    .mgroup = DEFAULT_MGROUP,
    .player_cmd = DEFAULT_PLAYERCMD
}

static void printhelp(void)
{
    printf("-P --port       指定接收端口\n
            -M --mgroup     指定多播组\n
            -p --player     指定播放器命令行\n 
            -H --help       显示帮助\n");
}

/*
 * -M --mgroup 指定多播组
 * -P --port   指定接收端口
 * -p --player 指定播放器
 * -H --help   显示帮助
 */
int main(int argc, char *argv[])
{
    int pd[2];
    struct sockaddr_in laddr;
    int c;
    int sd;
    int index = 0;
    struct ip_mreqn mreq;
    struct option argarr[] = {{"prot", 1, NULL, 'P'}, {"mgroup", 1, NULL, 'M'}, {"player", 1, NULL, 'p'}, {"help", 0, NULL, 'H'}, {NULL, 0, NULL, 0}};

    /*
     * 初始化级别：默认值，配置文件，环境变量，命令行参数
     */
    while (1)
    {
        c = getopt_long(argc, argv, "P:M:p:H", argarr, &index);
        if (c < 0)
            break;

        switch (c)
        {
            case 'P':
                client_conf.rcvport = optarg;
                break;
            case 'M':
                client_conf.mgroup = optarg;
                break;
            case 'p':
                client_conf.player_cmd = optarg;
                break;
            case 'H':
                printhelp();
                exit(0);
                break;
            default:
                abort();
                break;
        }
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
    //if error
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex("wlp8s0");

    if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    int val = 1;//true
    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) < 0)
    {
        perror("setsockopt()");
        exit(1);
    }

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.rcvport));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);

    if (bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    if (pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if (pid == 0)//child
    {
        //紫禁城：调用解码器
    }

    //parent
    
    //父进程：从网络上手包，发送给紫禁城
    
    //收节目单
    //选择频道
    //收频道包，发送给紫禁城

    exit(0);
}
