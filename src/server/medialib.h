/*
 * thr_medialib.h
 * Copyright (C) 2022 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef THR_MEDIALIB_H
#define THR_MEDIALIB_H

#define MP3_BITRATE 65536

struct mlib_listentry_st
{
    chnid_t chnid;
    char *desc;
};

int mlib_getchnlist(struct mlib_listentry_st **, int *);
int mlib_freechnlist(struct mlib_listentry_st *);

ssize_t mlib_readchn(chnid_t, void *, size_t);

#endif /* !THR_MEDIALIB_H */
