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
int open (const char *file_name);
struct file *fd_to_struct_filep(int fd);
int filesize (int fd);

#endif /* userprog/syscall.h */
