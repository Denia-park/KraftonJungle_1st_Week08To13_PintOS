#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "../lib/stdbool.h"
#include "../lib/kernel/stdio.h"

void syscall_init (void);
void check_address(void *addr);
void halt (void);
void exit (int status);
bool create (const char *file_name, unsigned initial_size);
bool remove (const char *file_name);
int write (int fd, const void *buffer, unsigned size);

#endif /* userprog/syscall.h */
