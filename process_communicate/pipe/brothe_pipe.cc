#include <string.h>
 
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
/*兄弟进程的无名管道*/
int main(void)
{
    pid_t pid  = -1;
    int ret    = -1;
    int status = -1;
    int fd[2]  = {0};
    char buf[128] = {0};
    int i = 0;
    //创建无名管道
    ret = pipe(fd);
    if (-1 == ret) {
        perror("pipe failed: ");
        goto _OUT;
    }
    //创建子进程
    for (i = 0; i < 2; i++) {
        pid = fork();
        if (0 > pid) {
            perror("fork failed: ");
            goto _OUT;
        }
        /*i=0则一个子进程写，i=1则另外一个子进程读*/
        //子进程写操作
        if (0 == pid) {
            if (0 == i) {
                printf("进入写管道的子进程\n");
                sleep(3);
                //先关闭读端，以免冲突
                close(fd[0]);
                //write pipe
                memset(buf, 0, sizeof(buf));
                ret = write(fd[1], "hello pipe", sizeof("hello pipe"));
                if(-1 == ret)
                {
                    perror("write failed: ");
                    goto _OUT;
                }
                printf("出写管道的子进程\n");
                // 此处必须结束，否则写管道进程会进行for循环的i=1的
                // 过程，这时，该进程空间的fd[0]是被关闭的，会报错。
                goto _OUT;
            }
            //子进程写操作
            if(1 == i)
            {
                printf("进入读管道的子进程\n");
                /*由于管道打开时默认以阻塞（block）模式打开，所以一直等
                 * 写进程沉睡3秒向管道中写入数据，读进程才从管道中读出
                 * 数据。通过fcntl函数，可以设置文件描述符的特性，改变
                 * 这种阻塞状态*/
                //先关闭写端，以免冲突
                close(fd[1]);
                //read pipe
                ret = read(fd[0], buf, sizeof(buf));
                if (-1 == ret) {
                	perror("read failed: ");
                    goto _OUT;
                }
                printf("ret=%d buf = %s\n",ret, buf);
                printf("出读管道的子进程\n");
                goto _OUT;
            }
        }
    } 
    // 父进程等待儿子并读
    // 父进程若先挂掉，则子进程成为僵死进程，脱离父进程独立存在。
    if (0 <  pid) {  
        printf("进入父进程\n");
        //wait child
        wait(&status);
        wait(&status);
    }
_OUT:   
	close(fd[0]);
	close(fd[1]);
	return ret;
}
