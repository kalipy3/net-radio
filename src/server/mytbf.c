/*
 * mytbf.c
 * Copyright (C) 2022 2022-05-26 11:04 kalipy <kalipy@debian>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

#include "mytbf.h"


mytbf_t *mytbf_init(int cps, int burst)
{

}

int mytbf_fetchtoken(mytbf_t *, int)
{

}

int mytbf_returntoken(mytbf_t *, int)
{

}

int mytbf_destroy(mytbf_t *)
{

}



