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

/*
 * -M --mgroup 指定多播组
 * -P --port   指定接收端口
 * -p --player 指定播放器
 * -H --help   显示帮助
 */
int main(int argc, char *argv[])
{
    /*
     * 初始化级别：默认值，配置文件，环境变量，命令行参数
     */
    getopt_long();

    socket();

    pipe();

    fork();

    //紫禁城：调用解码器
    //父进程：从网络上手包，发送给紫禁城

    exit(0);
}
