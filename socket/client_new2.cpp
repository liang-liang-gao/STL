/*
server.c  gll  2021/3/21
实现方式：epoll模型 RAII机制
功能：从标准输入接受数据，并发送给服务器端；从服务器端接收处理后的数据
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
//#include <cstring>
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
#include <ctype.h>
#include <iostream>
#include "RAII.h"
using namespace std;
#define MAX_EVENT 100

string noblockRead(int socket_fd) {
    char buf[5];
    //必须通过循环将stdin中的所有数据全部写入文件中
    int size;
    string ret;
    while(1) {
        memset(buf, 0, sizeof(buf)); 
        size = read(socket_fd, buf, sizeof(buf)-1);
        if(size < 0) {
            //判断是否是读完
            if(errno == EAGAIN) {
                //printf("read over!\n");
                break;
            }
            //否则出错
            printf("read failed!errno code is %d,errno message is '%s'\n",errno, strerror(errno));
        }
        buf[size] = '\0'; 
        if(buf[1] == '\0') {
            printf("please enter message to send:\n"); 
            return ret;
        }
        for(int i = 0; i < size; i++) {
            ret += buf[i];
            //cout<<ret[i]<<endl;
        }
    }
    return ret;
}

void process_stdin(int client_fd) {
    string ret = noblockRead(STDIN_FILENO);//从标准输入读取数据
    if(ret.size()) {
        int _size = ret.size();
        char* ans = new char[_size];
        strcpy(ans, ret.c_str());
        write(client_fd, ans, _size);
        printf("Bytes: %d\n", _size);
        printf("write data over!\n");
        delete ans;
    }

}

void process_server(int client_fd, int socket_fd) {
    string ret = noblockRead(client_fd);
    int size = ret.size();

    //保存这个client对应的server的地址和端口
    struct sockaddr_in peer_addr;
    int peer_len = sizeof(struct sockaddr_in);//必须这样写
    getpeername(socket_fd, (struct sockaddr *)&peer_addr, (socklen_t*)&peer_len); //获取对应server相应信息
    //保存server的ip和port
    char temp[1024];
    memset(temp, 0, sizeof(temp));
    const char* server_addr = inet_ntop(AF_INET, &peer_addr.sin_addr, temp, 1024);
    int server_port = ntohs(peer_addr.sin_port);
    if(size > 0) {
        //涉及到string，用cout输出
        cout<<"message recv "<<size<<"Bytes from server whose ip is "<<server_addr<<" and port is "<<server_port<<endl;
        cout<<"Bytes: "<<ret<<endl;
        //printf("message recv %dByte from server whose ip is %s and port is %d.\nBytes: %s\n", size, server_addr, server_port, ret);
    }
    else if(size < 0) {
        printf("recv failed!errno code is %d,errno message is '%s'\n",errno, strerror(errno));
        exit(1);
    }
    else {
        printf("server disconnect 1!\n");
        exit(1);
    }	
}
int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("format error!\n");
		printf("usage: server <address> <port>\n");
		exit(1);
    }
    //1.首先需要建立一个eventpool，当然，需要用文件描述符标识
    int eventpool_fd = epoll_create(10); //这个参数就是出初始化红黑树节点的个数
    if(eventpool_fd < 0) {
        perror("epoll create error:");
        exit(1);
    }
    //利用RAII思想
    FileRAII _event_fd(eventpool_fd);
    int event_fd = _event_fd.get();
    //2.接下来进行socket的创建和绑定及监听工作
    struct sockaddr_in client_addr;//地址
    client_addr.sin_family = AF_INET;//ipv4协议
    client_addr.sin_port = htons(atoi(argv[2]));//绑定端口
    if(!inet_aton(argv[1], (struct in_addr*)&client_addr.sin_addr.s_addr)) {//将点分十进制ip地址转换为对应形式
        perror("invalid ip addr:");
		exit(1); 
    }
    //1.创建套接字
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd < 0) {
        perror("socket create fail:");
        exit(1);
    }
    FileRAII _clit_fd(client_fd);
    int clit_fd = _clit_fd.get();
    //2.和server建立连接
	int size_sock = sizeof(client_addr);
	if(connect(clit_fd, (const struct sockaddr*)&client_addr, size_sock) < 0) {
		perror("connect error");
		//close(client_fd);
		exit(1); 
	}
    // int client_fd = init_client(argv);
    struct epoll_event event;//定义插入时的epoll事件
    struct epoll_event wait_event[MAX_EVENT];//定义最终返回的触发的事件描述符及其他信息

    //3.首先将标准输入以只读形式,边沿触发加入epoll
    //设置为非阻塞机制
    int flag = fcntl(STDIN_FILENO, F_GETFL);//返回阻塞类型
    flag |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flag);
    event.data.fd = STDIN_FILENO;
    event.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(event_fd, EPOLL_CTL_ADD, STDIN_FILENO, &event) < 0) {
        perror("epoll_ctl error:");
        //close(eventpool_fd);
        exit(1);
    }
    //4.将clit_fd以读入和套接字关闭的形式放入epoll，水平触发
    //设置为非阻塞机制
    flag = fcntl(clit_fd, F_GETFL);//返回阻塞类型
    flag |= O_NONBLOCK;
    fcntl(clit_fd, F_SETFL, flag);
    event.data.fd = clit_fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;//建立的连接请求对应的client_fd可读还可以接受关闭信息,边沿触发
    if(epoll_ctl(event_fd, EPOLL_CTL_ADD, clit_fd, &event) < 0) {
        perror("epoll_ctl error:");
        //close(eventpool_fd);
        exit(1);
    }
    struct sockaddr_in server;
    size_sock = sizeof(struct sockaddr_in);//地址长度
    char buf[1024];//保存接受数据和发送数据，使用前记得清空
    while(1) {
        int fd_size = epoll_wait(event_fd, wait_event, MAX_EVENT, -1);//水平阻塞触发
        if(fd_size < 0) {
            perror("perror wait error:");
            break;
        }
        for(int i = 0; i < fd_size; i++) {
            //从标准输入输入数据
            if(wait_event[i].data.fd == STDIN_FILENO) {//说明是标准输入  
                process_stdin(clit_fd);
                continue;
            }
            //和server断开连接
            if((wait_event[i].events & EPOLLRDHUP) == EPOLLRDHUP && wait_event[i].data.fd == clit_fd) {
                printf("server disconnect!\n");
                exit(1);
            }
            //收到server的回复
            if((wait_event[i].events & EPOLLIN) == EPOLLIN && wait_event[i].data.fd == clit_fd) {
                process_server(clit_fd, wait_event[i].data.fd);
                continue;   
            }
        }
    }
    return 0;
}