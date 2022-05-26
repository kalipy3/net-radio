#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "../include/proto.h"

#include "thr_list.h"
#include "server_conf.h"

static pthread_t tid_list;
static int nr_list_ent;
static struct mlib_listentry_st *list_ent;

static void *thr_list(void *p)
{
    int size;
    int ret;
    int totalsize;
    struct msg_list_st *entlistp;
    struct msg_listentry_st *entryp;

    totalsize = sizeof(chnid_t);

    for (int i = 0; i < nr_list_ent; i++)
    {
        totalsize += sizeof(struct msg_listentry_st) + strlen(list_ent[i].desc);
    }

    entlistp = malloc(totalsize);
    if (entlistp == NULL)
    {
        syslog(LOG_ERR, "malloc():%s.", strerror(errno));
        exit(1);
    }

    entlistp->chnid = LISTCHNID;
    entryp = entlistp->entry;

    for (int i = 0; i < nr_list_ent; i++)
    {
        size = sizeof(struct msg_listentry_st) + strlen(list_ent[i].desc);

        entryp->chnid = list_ent[i].chnid;
        entryp->len = htons(size);
        strcpy(entryp->desc, list_ent[i].desc);
        entryp = (void *)(((char *)entryp) + size);
    }

    while (1)
    {
        ret = sendto(serversd, entlistp, totalsize, 0, (void *)&sndaddr, sizeof(sndaddr));
        if (ret < 0)
        {
            syslog(LOG_WARNING, "sendto(serversd, entlistp..):%s", strerror(errno));
        } else {
            syslog(LOG_WARNING, "sendto(serversd, entlistp..):successed.");
        }

        sleep(1);
    }

}

int thr_list_create(struct mlib_listentry_st *listp, int nr_ent)
{
    int err;

    list_ent = listp;
    nr_list_ent = nr_ent;

    err = pthread_create(&tid_list, NULL, thr_list, NULL);
    if (err)
    {
        syslog(LOG_ERR, "pthread_create():%s.", strerror(err));
        return -1;
    }
    return 0;
}

int chr_list_destroy(void)
{
    pthread_cancel(tid_list);
    pthread_join(tid_list, NULL);
    return 0;
}
