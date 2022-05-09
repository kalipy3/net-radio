#ifndef PROTO_H
#define PROTO_H

#include <site_type.h>

#define DEFAULT_MGROUP      "224.2.2.2"//多播地址
#define DEFAULT_RCVPORT     "9999"

#define CHNNR               100//总频道数
#define LISTCHNID           0//节目单频道号
#define MINCHNID            1//最小频道号
#define MAXCHNID            (MINCHNID + CHNNR - 1)

#define MSG_CHANNEL_MAX     (65536 - 20 - 8)//整个msg_channel_st的最大大小。注意：20是ip包的包头长度，8是udp包的包头长度
#define MAX_DATA            (MSG_CHANNEL_MAX - sizeof(chnid_t))

#define MSG_LIST_MAX        (65536 - 20 - 8)
#define MAX_ENTRY           (MSG_LIST_MAX - sizeof(chnid_t));

struct msg_channel_st
{
    chnid_t chnid;//must between [MINCHNID, MAXCHNID]
    uint8_t data[1];//这里之所以不定义成data[0]，是因为有的c标准不支持数组长度为0的写法
} __attribute__((packed));

//1 music:descxxx
//2 sport:descxxxxx
//3 xxxxx:yyyy
//..
struct msg_listentry_st
{
    chnid_t chnid;
    uint16_t len;//本结构体长度
    uint8_t desc[1];
} __attribute__((packed));

struct msg_list_st
{
    chnid_t chnid;//must between LISTCHNID
    struct msg_listentry_st entry[1];

} __attribute__((packed));

#endif /* !PROTO_H */
