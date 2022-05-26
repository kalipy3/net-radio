/*
 * server_conf.h
 * Copyright (C) 2022 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef SERVER_CONF_H
#define SERVER_CONF_H

#define DEFAULT_MEDIADIR    "/home/kalipy/gg/media"
#define DEFAULT_IF  "eth0"

enum
{
    RUN_DAEMON = 1,//后台运行
    RUN_FOREGROUND//前台运行
};

struct server_conf_st
{
    char *rcvport;
    char *mgroup;
    char *media_dir;
    char runmode;
    char *ifname;//网卡
};

extern struct server_conf_st server_conf;
extern int serversd;
extern struct sockaddr_in sndaddr;

#endif /* !SERVER_CONF_H */
