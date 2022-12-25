#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

pid_t fork(void);

void unix_error(char *msg){ //Unix style error message
    fprintf(stderr, "Error: %s\n", msg);
    exit(0);
}


pid_t Fork(void){
    pid_t pid;

    if((pid = fork()) < 0)
        unix_error("Fork error");
    return pid;
}

int main(){
    pid_t pid;
    int x = 1;

    printf("Fork_test Start \n\n");

    pid = Fork();

    // child
    if(pid == 0){ 
        printf("child : x=%d\n\n", ++x);
        exit(0);
    }

    //parent
    printf("parent : x=%d\n\n", --x);
    printf("child PID : pid=%d\n\n", pid);
    exit(0);
}