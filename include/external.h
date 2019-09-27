#ifndef _EXTERNAL_H
#define _EXTERNAL_H

#include "main.h"
#include <stdbool.h>
#include <stddef.h>

void launch_job(job *j, int fg);
void launch_process(process *proc, pid_t pgid, int infile, int outfile,
                    bool fg);
void foreground(process *proc);
void background(process *proc);
void free_job(job *j);

#endif
