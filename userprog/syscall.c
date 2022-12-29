#include "userprog/syscall.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */

#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}

//주소 값이 유저 영역 주소 값인지 확인하고, 유저 영역을 벗어난 영역일 경우 프로세스 종료 exit(-1)*/
void 
check_address(void *addr)
{
	struct thread *curr = thread_current();
	if ( is_kernel_vaddr(addr) || addr == NULL ||
		pml4_get_page(curr->pml4, addr) == NULL ){
			exit(-1) ;
	}
}

/* The main system call interface */
void
syscall_handler (struct intr_frame *f UNUSED) {
	/* 유저 스택에 저장되어 있는 시스템 콜 넘버를 가져온다. */
	int sys_number = f->R.rax; // rax: 시스템 콜 넘버
    /* 
	인자 들어오는 순서:
	1번째 인자: %rdi
	2번째 인자: %rsi
	3번째 인자: %rdx
	4번째 인자: %r10
	5번째 인자: %r8
	6번째 인자: %r9 
	*/
	switch(sys_number) {
		case SYS_HALT:
			halt();
			break;
		case SYS_EXIT:
			exit(f->R.rdi);
			break;
		// case SYS_FORK:
		// 	fork(f->R.rdi);	
			// break;	
		// case SYS_EXEC:
		// 	exec(f->R.rdi);
			// break;
		// case SYS_WAIT:
		// 	wait(f->R.rdi);
			// break;
		case SYS_CREATE:
			create((char *) f->R.rdi, f->R.rsi);		
			break;
		case SYS_REMOVE:
			remove((char *) f->R.rdi);
			break;		
		case SYS_OPEN:
			open(f->R.rdi);	
			break;	
		// case SYS_FILESIZE:
		// 	filesize(f->R.rdi);
			// break;
		// case SYS_READ:
		// 	read(f->R.rdi, f->R.rsi, f->R.rdx);
			// break;
		case SYS_WRITE:
			write(f->R.rdi, (void *) f->R.rsi, f->R.rdx);
			break;		
		// case SYS_SEEK:
		// 	seek(f->R.rdi, f->R.rdx);
			// break;		
		// case SYS_TELL:
		// 	tell(f->R.rdi);	
			// break;	
		// case SYS_CLOSE:
		// 	close(f->R.rdi);
			// break;	
	}
	printf ("system call! : %d \n", sys_number);
	thread_exit ();
}

void
halt (void) {
	power_off ();
}

void
exit (int status) {
	struct thread *curr = thread_current();
	curr->exit_status = status;

	thread_exit();
}
bool
create (const char *file_name, unsigned initial_size) {
	check_address ((void *)file_name);
	return filesys_create((char *) file_name, initial_size);
}

bool
remove (const char *file_name) {
	check_address ((void *)file_name);
	return filesys_remove((char *) file_name);
}int

 /* 파일을 현재 프로세스의 fdt에 추가 */
int add_file_to_fd_table(struct file *file) {
	struct thread *t = thread_current();
	struct file **fdt = t->file_descriptor_table;
	int fd = t->fdidx; //fd값은 2부터 출발
	
	while (t->file_descriptor_table[fd] != NULL && fd < FDT_COUNT_LIMIT) {
		fd++;
	}

	if (fd >= FDT_COUNT_LIMIT) {
		return -1;
	}

	t->fdidx = fd;
	fdt[fd] = file;
	return fd;
}

int
open (const char *file_name) {
	check_address ((void *)file_name);
	struct file * file_obj = filesys_open(file_name);

	if(file_obj == NULL){
		return -1;
	}

	int fd = add_file_to_fd_table(file_obj); // 만들어진 파일을 스레드 내 fdt 테이블에 추가

	// 만약 파일을 열 수 없으면] -1을 받음
	if (fd == -1) {
		file_close(file_obj);
	}

	return fd;
}

write (int fd, const void *buffer, unsigned size) {
	if(fd == STDOUT_FILENO){
		putbuf(buffer, size);
	}
	return size;
}
