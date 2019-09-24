#ifndef _EXTERNAL_H
#define _EXTERNAL_H

#include "main.h"
#include <stdbool.h>
#include <stddef.h>

void launch_job(job *j, int fg);
void foreground(process *proc);
void background(process *proc);
void free_job(job *j);

#endif
