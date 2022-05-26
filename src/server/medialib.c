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

static struct channel_context_st *path2entry(const char *path)
{
    // path/desc.text  path/*.mp3
    char pathstr[PATHSIZE];
    char linebuf[LINEBUFSIZE];
    FILE *fp;
    struct channel_context_st *me;
    static chnid_t curr_id = MINCHNID;

    strncpy(pathstr, path, PATHSIZE);
    strncat(pathstr, "/desc.text", PATHSIZE);

    fp = fopen(patstr, "r");
    if (fp == NULL)
    {
        syslog(LOG_INFO, "%s is not a channel dir(Can't find desc.text)", path);
        return NULL;
    }
    
    if (fgets(linebuf, LINEBUFSIZE, fp) == NULL)
    {
        syslog(LOG_INFO, "%s is not a channel dir(Can't find desc.text)", path);
        fclose(fp);
        return NULL;
    }

    fclose(fp);

    me = malloc(sizeof(*me));
    if (me == NULL)
    {
        syslog(LOG_ERR, "malloc():%s\n", strerror(errno));
        return NULL;
    }

    me->tbf = mytbf_init(MP3_BITRATE/8, MP3_BITRATE/8*10);
    if (me->tbf == NULL)
    {
        syslog(LOG_ERR, "mytbf_init():%s\n", strerrno(errno));
        free(me);
        return NULL;
    }

    me->desc = strdup(linebuf);

    strncpy(pathstr, path, PATHSIZE);
    strncat(pathstr, "/*.mp3", PATHSIZE);
    if (glob(pathstr, 0, NULL, &me->mp3glob) != 0)
    {
        curr_id++;
        syslog(LOG_ERR, "%s is not a channel dir(Can't find mp3 files)", path);
        free(me);
        return NULL;
    }

    me->pos = 0;
    me->offset = 0;
    me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY);
    if (me->fd < 0)
    {
        syslog(LOG_WARNING, "%s open failed.", me->mp3glob.gl_pathv[me->pos]);
        free(me);
        return NULL;
    }

    me->chnid = curr_id;
    curr_id++;

    return me;
}

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
        res = path2entry(globres.gl_pathv[i]);
        if (res != NULL)
        {
            syslog(LOG_DEBUG, "path2entry() returned [%d %s]", res->chnid, res->desc);
            memcpy(channel + res->chnid, res, sizeof(*res));
            ptr[num].chnid = res->chnid;
            ptr[num].desc = strdup(res->desc);
        }
        num++;
    }

    *result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
    if (*resnum == NULL)
    {
        syslog(LOG_ERR, "realloc() failed.");
    }

    *resnum = num;

    return 0;
}

int mlib_freechnlist(struct mlib_getchnlist *ptr)
{
    free(ptr);
}

ssize_t mlib_readchn(chnid_t, void *, size_t)
{

}
