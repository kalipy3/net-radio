/*
 * thr_channel.h
 * Copyright (C) 2022 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef THR_CHANNEL_H
#define THR_CHANNEL_H

#include "medialib.h"

int chr_channel_create(struct mlib_listentry_st *);
int chr_channel_destroy(struct mlib_listentry_st *); 
int chr_channel_destroyall(void); 

#endif /* !THR_CHANNEL_H */
