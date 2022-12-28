#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "../lib/stdbool.h"

void syscall_init (void);
void check_address(void *addr);
void halt (void);
void exit (int status);
bool create (const char *file_name, unsigned initial_size);

#endif /* userprog/syscall.h */
