#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <resolv.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/epoll.h>

int main (int argc, char * argv[])
{
	int s_fd = -1, c_fd = -1, e_fd = -1,pid; 
	socklen_t addr_len;
	struct sockaddr_in s_addr, c_addr;
	char buf[1024];
	ssize_t size = 0;
	struct epoll_event e_event;   	/* 监听事件 */ 
    struct epoll_event wait_event[10];  /* 监听事件结果 */
	int max_fd;
	int ret = 0;
	
	if(argc != 3)
	{
		printf("format error!\n");
		printf("usage: server <address> <port>\n");
		exit(1)	;
	}
	/* epoll 监听参数 */
	e_fd = epoll_create(10);	/* 最大监听10个事件 */
	if(e_fd < 0)
	{
		perror("epoll create error");
		close(s_fd); 
		exit(1);
	}
	/* 服务器端地址 */ 
	s_addr.sin_family = AF_INET;
	s_addr.sin_port   = htons(atoi(argv[2]));
	if(!inet_aton(argv[1], (struct in_addr *) &s_addr.sin_addr.s_addr))
	{
		perror("invalid ip addr:");
		exit(1); 
	}	
	
	/* 创建socket */
	if((s_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("socket create failed:"); 
		exit(1); 
	} 

	/* 端口重用，调用close(socket)一般不会立即关闭socket，而经历“TIME_WAIT”的过程 */
	int reuse = 0x01;
	if(setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) < 0)
	{
		perror("setsockopt error");
		close(s_fd);
		exit(1); 
	}
	
	/* 绑定地址 */
	if(bind(s_fd, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0)
	{
		perror("bind error");
		close(s_fd);
		exit(1); 
	}
	
	/* 监听socket */
	if(listen(s_fd, 5) < 0)
	{
		perror("listen error");
		close(s_fd);
		exit(1); 
	}
	addr_len = sizeof(struct sockaddr);


/*	e_event.data.fd = STDIN_FILENO;   
    e_event.events = EPOLLIN;
	ret = epoll_ctl(e_fd, EPOLL_CTL_ADD, STDIN_FILENO, &e_event);
	if(ret < 0)
	{
		perror("epoll_ctl error");
		close(s_fd); 
		close(e_fd); 
		exit(1);
	}
*/
	e_event.data.fd = s_fd;     
    e_event.events = EPOLLIN;
	ret = epoll_ctl(e_fd, EPOLL_CTL_ADD, s_fd, &e_event);
	if(ret < 0)
	{
		perror("epoll_ctl error");
		close(s_fd); 
		close(e_fd); 
		exit(1);
	}
	max_fd = 8;

	for(;;)
	{
		ret = epoll_wait(e_fd,wait_event,max_fd,-1);	/* 阻塞 */

		if(ret < 0)
		{
			perror("epoll_wait error");
			break;
		}
		else if(ret == 0)
		{
			continue;
		}

		for(size = 0; size < ret;size++)
		{
			if(((wait_event[size].events & EPOLLIN) == EPOLLIN) && (wait_event[size].data.fd == s_fd))
			{
/*				if(c_fd>0)
				{
					continue;
				}
				printf("waiting client connecting\r\n");
				
*/
				c_fd = accept(s_fd, (struct sockaddr*)&c_addr, (socklen_t *)&addr_len);
				
				if(c_fd < 0)
				{
					perror("accept error");
					break; 
				}
				else//收到连接请求
				{
					printf("connected with ip: %s and po	rt: %d\n", inet_ntop(AF_INET,&c_addr.sin_addr, buf, 1024), ntohs(c_addr.sin_port));
				}
				e_event.data.fd = c_fd;     
	    		e_event.events = EPOLLIN | EPOLLET;
				ret = epoll_ctl(e_fd, EPOLL_CTL_ADD, c_fd, &e_event);//将这个连接请求加入eventpool
				if(ret < 0)
				{
					perror("epoll_ctl error");
					close(s_fd);
					break; 
				}

				//max_fd = 3;
				continue;
			}
			else 
			{
				memset(buf, 0, sizeof(buf)); 
				size = read(wait_event[size].data.fd, buf, sizeof(buf) - 1);
				if(size > 0)
				{
					printf("message recv %dByte: \n%s\n",size,buf);
					printf("用户数据：%s\n", (char*)wait_event[size].data.ptr);
				}
				else if(size < 0)
				{
					printf("recv failed!errno code is %d,errno message is '%s'\n",errno, strerror(errno));
					goto quit;
				}
				else
				{
					printf("client disconnect 1!\n");
					continue;
				}
			}
/*
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
		
/*
			if((wait_event[size].events & EPOLLRDHUP) == EPOLLRDHUP && wait_event[size].data.fd == c_fd)
			{/* disconnect */
//				printf("client disconnect 0!\n");
//				goto quit;
//			}

/*			if(((wait_event[size].events & EPOLLIN) == EPOLLIN && wait_event[size].data.fd == c_fd))
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
					printf("client disconnect 1!\n");
					goto quit;
				}	
			}
*/
		}
	}
quit:
	close(s_fd); 
	close(c_fd);
	close(e_fd);

	return 0;
}



