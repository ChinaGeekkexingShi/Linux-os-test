#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
 
/*int pipe(int filedes[2]);无名管道,返回0成功，返回-1失败*/
/*fd[0]为读端，fd[1]为写端，从尾部写头部读*/
/*父子进程的无名管道*/
int main(void) {
    pid_t pid  = -1;
    int ret    = -1;
    int status = -1;
    int fd[2]  = {0};
    char buf[128] = {0};
    
    //创建管道
    ret = pipe(fd);
    if (-1 == ret) {
        perror("pipe failed: ");
        goto _OUT;
    }

    //创建子进程
    pid = fork();
    if (-1 == (ret=pid)) {
        perror("fork failed: ");
        goto _OUT;
    }

    //子进程写操作
    if (0 == pid) {
        //先关闭读端，以免冲突
        close(fd[0]);
        //write pipe
        ret = write(fd[1], "howaylee", sizeof("howaylee"));
        if(-1 == ret)
        {
            perror("write failed: ");
            goto _OUT;
        }
    }
	//父进程等待儿子并读
    else {
        //wait child
        wait(&status);
        //先关闭写端，以免冲突
        close(fd[1]);
        //read pipe
        ret = read(fd[0], buf, sizeof(buf));
        if(-1 == ret)
        {
            perror("read failed: ");
            goto _OUT;
        }
 
        printf("buf = %s\n", buf);
    }

_OUT:   
    return ret;
}
