#ifndef CLIENT_H
#define CLIENT_H

#define DEFAULT_PLAYERCMD  "/usr/bin/mpg123 - > /dev/null"//-表示从stdin获取输入数据

struct client_conf_st
{
    char *rcvport;
    char *mgroup;
    char *player_cmd;
};

extern struct client_conf_st clent_conf;

#endif /* !CLIENT_H */
