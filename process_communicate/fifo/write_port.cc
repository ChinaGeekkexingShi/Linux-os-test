/*
 * *功能描述：通过有名管道实现两个进程之间的双向通信，实现对话功能。本部分从fifo1中写入信息，
 * *       发送给另一个程序，同时用子进程从另一个管道fifo2中等待读取信息.
 * */
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define N 20
int main(void)
{
    int i;
    char getstr[N];
    char redstr[N];
    char fname1[]="fmsg1";
    int fid1, fifo_create_status[2];
    pid_t pid;
    
    printf("程序1：\n");
    
	// 删除文件，若还有其他打开链接，则仅删除链接。
	// 此处确保该进程没有以这个命名打开文件。
    unlink(fname1);

	// 根据文件名创建有名管道，第二个参数为权限设置.
    fifo_create_status[0]=mkfifo(fname1, 0666);
    
	// 判断是否建成fifo管道
    if (fifo_create_status[0] < 0) {
        perror("mkfifo");
        exit(-1);       //错误并退出
    }
    else {
        printf("fifo made succeed!\n"); //test
    }
    
    pid = fork();
    fid1 = open(fname1,O_WRONLY);
    if(fid1==-1)
    {
        perror("Parent file opened error");
        exit(-1);
    }
    printf("file opend succeed!\n");
    // 写进程向管道1中写入信息,写进程等待子进程切换过来后，写入发送的消息。
    // 进程随后切换到子进程，子进程打印消息。继续等待，如此循环，一直到写进程
    // 输入结束符，并驱动子进程结束。
    int count = 0;
    while(1)
    {
        printf("**********************\n");
        printf("第%d次进入写进程\n", ++count);
        printf("请输入要发送的信息(输入end/END退出)：\n");
        fgets(getstr, N, stdin);
        
        if(write(fid1,getstr,strlen(getstr))<0)  //发送消息
        {
            perror("Parent write error");
	        break;
        }
        else {
            printf("信息已发送.\n");
	        // strlen不会算入最后的结束符\0,此处清除掉最后的回车符
	        // 回车即是确定提交上次输入，又是下次输入的一个字符，所以需要清楚掉。
	        getstr[strlen(getstr)-1] = '\0';
            if((strcmp(getstr,"END") == 0) || (strcmp(getstr,"end")==0))    //输入end退出
            {
                printf("写进程准备退出程序.\n");
                break;
            }
        }
    }
    close(fid1);
    unlink(fname1);
	// 写进程在这一步有时会出现进程无法正常退出的情况，目前不知道是什么原因？？？
    return 0;
}
