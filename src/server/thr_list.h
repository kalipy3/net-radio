/*
 * thr_list.h
 * Copyright (C) 2022 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef THR_LIST_H
#define THR_LIST_H

#include "medialib.h"

int thr_list_create(struct mlib_listentry_st *, int);
int thr_list_destroy(void);

#endif /* !THR_LIST_H */
