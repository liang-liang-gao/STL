#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/epoll.h>

int main (int argc, char * argv[])
{ 
	int c_fd = -1,e_fd = -1,pid; 
	int ret = 0;
	struct sockaddr_in s_addr;
	socklen_t addr_len;
	char buf[1024]; 
	ssize_t size;
	struct epoll_event e_event;   		/* 监听事件 */ 
    struct epoll_event wait_event[10];  /* 监听事件结果 */
	int max_fd; 						/* 监听文件描述符中最大的文件号 */

	if(argc != 3)
	{
		printf("format error!\n");
		printf("usage: client <address> <port>\n");
		exit(1);
	}
	
	/* 创建socket */
	c_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(c_fd < 0)
	{
		perror("socket create failed");
		return -1;
	} 
 
	/* 服务器端地址 */ 
	s_addr.sin_family 	= AF_INET;
	s_addr.sin_port 	= htons(atoi(argv[2]));
	if(!inet_aton(argv[1], (struct in_addr *) &s_addr.sin_addr.s_addr))
	{
		perror("invalid ip addr");
		exit(1); 
	}

	/* 连接服务器*/
	addr_len = sizeof(s_addr);
	ret = connect(c_fd, (struct sockaddr*)&s_addr, addr_len); 
	if(ret < 0)
	{
		perror("connect server failed");
		exit(1);  
	} 

	/* epoll 监听参数 */
	e_fd = epoll_create(10);	/* 最大监听10个事件 */
	if(e_fd < 0)
	{
		perror("epoll create error");
		exit(1);
	}
	e_event.data.fd = STDIN_FILENO;   
    e_event.events = EPOLLIN;
	ret = epoll_ctl(e_fd, EPOLL_CTL_ADD, STDIN_FILENO, &e_event);
	if(ret < 0)
	{
		perror("epoll_ctl error");
		close(e_fd); 
		exit(1);
	}

	e_event.data.fd = c_fd;     
    e_event.events = EPOLLIN | EPOLLRDHUP | EPOLLPRI;
	ret = epoll_ctl(e_fd, EPOLL_CTL_ADD, c_fd, &e_event);
	if(ret < 0)
	{
		perror("epoll_ctl error");
		close(e_fd); 
		exit(1);
	}
	max_fd = 2;

	for(;;)
	{
		ret = epoll_wait(e_fd,wait_event,max_fd+1,-1);	/* 阻塞 */

		if(ret < 0)
		{
			perror("epoll_wait error");
			break;
		}
		else if(ret == 0)
		{
			continue;
		}
		//printf("epoll out %d\n",ret);
		for(size = 0; size < ret;size++)
		{
			if((wait_event[size].events & EPOLLIN) == EPOLLIN && wait_event[size].data.fd == STDIN_FILENO)
			{
				fflush(stdout);
				memset(buf, 0, sizeof(buf)); 
				size = read(STDIN_FILENO, buf, sizeof(buf) - 1); 	
				if(size > 0) 
				{
					buf[size-1] = '\0'; 
				} 
				else
				{
					perror("read stdin error"); 
					goto quit;
				}
				if(!strncmp(buf, "quit", 4))
				{
					printf("close the connect!\n");
					goto quit;
				}
				if(buf[0] == '\0')
				{
					printf("please enter message to send:\n"); 
					continue;
				}
				size = write(c_fd, buf, strlen(buf)); 
				if(size <= 0)
				{
					printf("message'%s' send failed!errno code is %d,errno message is '%s'\n",buf, errno, strerror(errno));
					goto quit;
				}
				printf("please enter message to send:\n");
			}
		
			if((wait_event[size].events & EPOLLRDHUP) == EPOLLRDHUP && wait_event[size].data.fd == c_fd)
			{/* disconnect */
				printf("server disconnect 0!\n");
				goto quit;
			}

			if(((wait_event[size].events & EPOLLIN) == EPOLLIN && wait_event[size].data.fd == c_fd))
			{
				memset(buf, 0, sizeof(buf)); 
				size = read(c_fd, buf, sizeof(buf) - 1);
				if(size > 0)
				{
					printf("message recv %dByte: \n%s\n",size,buf);
				}
				else if(size < 0)
				{
					printf("recv failed!errno code is %d,errno message is '%s'\n",errno, strerror(errno));
					goto quit;
				}
				else
				{
					printf("server disconnect 1!\n");
					goto quit;
				}	
			}
		}
	}
	
quit:	
	close(c_fd); 
	close(e_fd);
	return 0; 
}

