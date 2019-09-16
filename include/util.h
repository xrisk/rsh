#ifndef _UTIL_H
#define _UTIL_H

#include "main.h"

void insert_job(job *j);
int update_status(pid_t pid, int status);
void wait_for_job(job *j);
void print_job_table();
bool check_stopped(job *j);
job_entry *get_job(int req);

#endif
