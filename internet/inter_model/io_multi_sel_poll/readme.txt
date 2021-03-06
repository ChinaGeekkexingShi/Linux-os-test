**io多路复用select+poll方式
多进程模型和多线程(线程池)模型每个进程/线程只能处理一路IO，在服务器并
发数较高的情况下，过多的进程/线程会使得服务器性能下降。而通过多路IO复
用，能使得一个进程同时处理多路IO，提升服务器吞吐量。
在Linux支持epoll模型之前，都使用select/poll模型来实现IO多路复用。

**以select为例，其核心代码如下：
bind(listenfd);
listen(listenfd);
FD_ZERO(&allset);
for(;;){  //该循环应该是一个死循环，一直在监听各个文件描述符。
    FD_SET(listenfd, &allset); // 每次都必须加入connect fd，检测是否有新的链接产生。
    select(...);  // 第一次循环，监听的仅仅是listenfd，后续会不断加入tcp链接的fd。当某个fd变化发生时，进行完处理，set
                  // bit位会被置0，等待下一次事件。
    if (FD_ISSET(listenfd, &rset)) {    /*有新的客户端连接到来*/
        clifd = accept();
        cliarray[] = clifd;       /*保存新的连接套接字*/
        FD_SET(clifd, &allset);  /*将新的描述符加入监听数组中*/
    }
    for(;;){    /*这个for循环用来检查所有已经连接的客户端是否由数据可读写*/
        fd = cliarray[i];
        if (FD_ISSET(fd , &rset))
            dosomething();  //处理完后，不会进行FD_SET设置，等待下次事件到来，循环整个过程。
    }
}

首先需要学习select函数。详细博客介绍：http://blog.csdn.net/turkeyzhou/article/details/8609360


**select IO多路复用同样存在一些缺点，罗列如下：
1、单个进程能够监视的文件描述符的数量存在最大限制，通常是1024，当然可
   以更改数量，但由于select采用轮询的方式扫描文件描述符，文件描述符数
   量越多，性能越差；(在linux内核头文件中，有这样的定义：#define __FD_SETSIZE    1024)
2、内核 / 用户空间内存拷贝问题，select需要复制大量的句柄数据结构，产生巨大的开销；
3、select返回的是含有整个句柄的数组，应用程序需要遍历整个数组才能发现哪些句柄发生了事件；
4、select的触发方式是水平触发，应用程序如果没有完成对一个已经就绪的文件描述符进
   行IO操作，那么之后每次select调用还是会将这些文件描述符通知进程。
   
**相比select模型，poll使用链表保存文件描述符，因此没有了监视文件数量的限制，但其他三个缺点依然存在。
拿select模型为例，假设我们的服务器需要支持100万的并发连接，则在__FD_SETSIZE 为1024的情况下，
则我们至少需要开辟1k个进程才能实现100万的并发连接。除了进程间上下文切换的时间消耗外，从内
核/用户空间大量的无脑内存拷贝、数组轮询等，是系统难以承受的。因此，基于select模型的服务器
程序，要达到10万级别的并发访问，是一个很难完成的任务。

**
select如何应对send时，客户端迟迟不接受数据，程序阻塞在dosomething处，无法继续运行？？？!!多用于短链接。
select如何解决，在dosomething时，业务处理逻辑涉及的一些io处理或调用其他服务（数据库、子系统）
的盲等行为？？？因为select的处理方式相当于是强行串行化，在同步阻塞的基础上优化了等待请求
数据来临的阻塞时间。？？？？这时候就是多线程多进程能够解决的问题.!!!!这也正是select的缺点：
该模型将事件探测和事件响应夹杂在一起，一旦事件响应的执行体庞大，则对整个模型是灾难性的。
幸运的是，有很多高效的事件驱动库可以屏蔽上述的困难，常见的事件驱动库有 libevent 库，还
有作为 libevent 替代者的 libev 库。这些库会根据操作系统的特点选择最合适的事件探测接口，
并且加入了信号 (signal) 等技术以支持异步响应，这使得这些库成为构建事件驱动模型的不二选择。

单线程+多work（与CPU核数一致）的方式。
