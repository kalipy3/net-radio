#include <stdio.h>
#include <stdlib.h>

#include "medialib.h"

#define PATHSIZE 1024

struct channel_context_st
{
    chnid_t chnid;
    char *desc;
    glob_t mp3glob;
    int pos;
    int fd;
    off_t offset;
    mytbf_t *tbf;
};

static struct channel_context_st channel[MAXCHNID + 1];

int mlib_getchnlist(struct mlib_getchnlist **result, int *resnum)
{
    char path[PATHSIZE];
    glob_t globres;
    int num = 0;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;

    for (int i = 0; i < MAXCHNID + 1; i++)
    {
        channel[i].chnid = -1;    
    }

    snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);

    if (glob(path, 0, NULL, &globres))
    {
        return -1;
    }

    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if (ptr == NULL)
    {
        syslog(LOG_ERR, "malloc() error.");
        exit(1);
    }
    
    for (int i = 0; i < globres.gl_pathc; i++)
    {
        path2entry(globres.gl_pathv[i]);
        num++;
    }

    *result = ;

    *resnum = num;

    return 0;
}

int mlib_freechnlist(struct mlib_getchnlist *)
{

}

ssize_t mlib_readchn(chnid_t, void *, size_t)
{

}
