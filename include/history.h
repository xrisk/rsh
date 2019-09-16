#ifndef _HISTORY_H
#define _HISTORY_H

#include "main.h"

void add_history_entry(char *line);
void show_history(process *p);
void initialize_history(void);
void free_history(void);
void persist_history(void);

#endif
