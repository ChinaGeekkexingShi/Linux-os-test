1、有名管道的阻塞模式，可以理解为一边生产，一边消耗，管道只是一个中间存储的仓库。当消费端拿走上次
   生产的物料，而生产端又没有新物料产生时，消费端不会再去访问仓库。这就是阻塞模式，无名管道也是这样，
   之前想的用信号去控制读的想法是错误的。如果写端未写，读端会一直等待，直到写端写入数据为止。所有的
   虚拟文件又都可以通过fcntl函数来设置文件的特性，关闭阻塞模式。磁盘文件类虚拟文件和管道、套接字等
   是不同的，它的读写不会默认为阻塞模式，而需要通过信号量或其他方式进行控制。关于阻塞IO和非阻塞IO，
   详细介绍：http://www.cnblogs.com/TianFang/archive/2013/01/22/2870663.html  。该文章很好的引入了：
   缓冲、阻塞、轮训、select函数等问题。以便后续进一步研究。
  
