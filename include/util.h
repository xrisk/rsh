#ifndef _UTIL_H
#define _UTIL_H

#include "main.h"

void insert_job(job *j);
int update_status(pid_t pid, int status);
void wait_for_job(job *j);

#endif
