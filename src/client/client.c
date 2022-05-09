#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include "client.h"
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
    int c;
    int index = 0;
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

    socket();

    pipe();

    fork();

    //紫禁城：调用解码器
    //父进程：从网络上手包，发送给紫禁城

    exit(0);
}
