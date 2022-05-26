#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
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
};

static void printhelp(void)
{
    printf("-P --port       指定接收端口\n\
            -M --mgroup     指定多播组\n\
            -p --player     指定播放器命令行\n\
            -H --help       显示帮助\n");
}

static ssize_t writen(int fd, const char *buf, size_t len)
{
    int pos = 0;
    int ret;

    while (len > 0)
    {
        ret = write(fd, buf + pos, len);
        if (ret < 0)
        {
            if (errno == EINTR)
                continue;
            perror("write()");
            return -1;
        }
        len -= ret;
        pos += ret;
    }

    return pos;
}

/*
 * -M --mgroup 指定多播组
 * -P --port   指定接收端口
 * -p --player 指定播放器
 * -H --help   显示帮助
 */
int main(int argc, char *argv[])
{
    pid_t pid;
    int ret;
    int len;
    int pd[2];
    struct sockaddr_in laddr, serveraddr, raddr;
    socklen_t serveraddr_len, raddr_len;
    int c;
    int sd;
    int index = 0;
    struct ip_mreqn mreq;
    int choosenid;
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
        close(sd);
        close(pd[1]);
        dup2(pd[0], 0);//pipe的读端转成标准输入
        if (pd[0] > 0)//pd[0]本身不是标准输入，才关闭pd[0]
            close(pd[0]);

        execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL);
        perror("execl()");
        exit(1);
    }

    //parent
    //父进程：从网络上手包，发送给紫禁城

    //收节目单
    struct msg_list_st *msg_list;
    msg_list = malloc(MSG_LIST_MAX);
    if (msg_list == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    serveraddr_len = sizeof(serveraddr);
    while (1)
    {
        len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&serveraddr, &serveraddr_len);
        if (len < sizeof(struct msg_list_st))
        {
            fprintf(stderr, "message is too small\n");
            continue;
        }
        if (msg_list->chnid != LISTCHNID)
        {
            fprintf(stderr, "chnid is not match\n");
            continue;
        }
        break;
    }

    //打印节目单并选择频道
    struct msg_listentry_st *pos;
    for (pos = msg_list->entry; (char *)pos < (((char *)msg_list) + len); pos = (void *)(((char *)pos) + ntohs(pos->len)))
    {
        printf("channel %d : %s\n", pos->chnid, pos->desc);
    }

    free(msg_list);

    //选择频道
    puts("Please enter:");
    while (ret < 1)
    {
        ret = scanf("%d", &choosenid);
        if (ret != 1)
            exit(1);
    }

    //收频道包，发送给紫禁城
    fprintf(stdout, "chosenid = %d\n", ret);
    
    struct msg_channel_st *msg_channel;
    msg_channel = malloc(MSG_CHANNEL_MAX);
    if (msg_channel == NULL)
    {
        perror("malloc()");
        exit(1);
    }

    raddr_len = sizeof(raddr);
    while (1)
    {
        len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &raddr_len);
        if (raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || raddr.sin_port != serveraddr.sin_port)
        {
            fprintf(stderr, "Ignore: address not math\n");
            continue;
        }

        if (len < sizeof(struct msg_channel_st))
        {
            fprintf(stderr, "Ignore: message too small\n");
            continue;
        }

        if (msg_channel->chnid == choosenid)
        {
            fprintf(stdout, "accepted msg: %d recieved\n", msg_channel->chnid);
            if (writen(pd[1], msg_channel->data, len - sizeof(chnid_t)) < 0)
                exit(1);
        }
    }

    free(msg_channel);
    close(sd);

    exit(0);
}
