
/*
server.c  gll  2021/3/21
实现方式：epoll模型 
功能：从客户端接收数据，并将其中小写字符转化为大写，重新发给客户端
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
#include "RAII.h"
using namespace std;
//#include <string.h>
//#include <file>
#define MAX_EVENT 1000//有上限
// class FileRAII {
// private:
//     int fd;
// public:
//     explicit FileRAII(int _fd = -1) : fd(_fd) {}
//     void change(int temp) {
//         fd = temp;
//     }
//     ~FileRAII() { if(fd != -1) close(fd); }
//     int get() { return fd; }
// };
//初始化server.c的socket,以及开启监听
// int init_server(char *argv[]) {
//     struct sockaddr_in server_addr;//地址
//     server_addr.sin_family = AF_INET;//ipv4协议
//     server_addr.sin_port = htons(atoi(argv[2]));//绑定端口
//     if(!inet_aton(argv[1], (struct in_addr*)&server_addr.sin_addr.s_addr)) {//将点分十进制ip地址转换为对应形式
//         perror("invalid ip addr:");
// 		exit(1); 
//     }
//     //1.创建套接字
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if(server_fd < 0) {
//         perror("socket create fail:");
//         exit(1);
//     }
//     FileRAII ser_fd(server_fd);//！！！利用RAII
//     //2.端口重用，如果不这样设置的话，建立TCP连接后，将无法再监听其他的客户端
// 	int reuse = 0x01;
// 	if(setsockopt(ser_fd.get(), SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) < 0) {
// 		perror("setsockopt error");
// 		// close(server_fd);
// 		exit(1); 
// 	}
//     //3.绑定套接字(ip,端口)
//     if(bind(ser_fd.get(), (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//         perror("bind error:");
//         // close(server_fd);
//         exit(1);
//     }
//     //4.开始监听
//     if(listen(ser_fd.get(), 127) < 0) {
//         perror("listen error");
//         //close(server_fd);
//         exit(1);
//     }
//     return ser_fd.get();//返回服务器端的连接套接字
// }

void process_data(int socket_fd) {
    int bytes;
    char buf[1024];
    //char ret[1024];//返回数据
    string ret;
    //int num = 0;
    char* s = buf;
    int size;
    //保存这个socket对应的客户端的地址和端口
    struct sockaddr_in peer_addr;
    int peer_len = sizeof(struct sockaddr_in);//必须这样写
    getpeername(socket_fd, (struct sockaddr *)&peer_addr, (socklen_t *)&peer_len); //获取对应客户端相应信息
    while(1) {
        memset(buf, 0, sizeof(buf)); 
        size = read(socket_fd, buf, sizeof(buf)-1);
        //得到客户端数据
        char temp[1024];
        memset(temp, 0, sizeof(temp));

        const char* client_addr = inet_ntop(AF_INET, &peer_addr.sin_addr, temp, 1024);//加了const
        int client_port = ntohs(peer_addr.sin_port);
        if(size < 0) {
            //判断是否是读完
            if(errno == EAGAIN) {
                printf("read over!\n");
                break;
            }
            //否则出错
            printf("recv failed!errno code is %d,errno message is '%s'\n",errno, strerror(errno));
        }
        if(size == 0) {//断开连接
			printf("client disconnect whose ip is %s and port is %d!\n", client_addr, client_port);
			//exit(1);
            return ;
        }

        printf("message recv %dByte from client whose ip is %s and port is %d.\nBytes: %s\n", size, client_addr, client_port, buf);
        //memset(ret, 0, sizeof(ret));
        for(int i = 0; i < strlen(buf); i++) {
            if(buf[i] >= 'a' && buf[i] <= 'z') {
                //ret[num] = toupper(buf[i]);
                ret += toupper(buf[i]);
            }
            else {
                //ret[num] = buf[i];
                ret += buf[i];
            }
            //num++;
        }
    }
    //读取完毕，将大写数据发送给client
    int _size = ret.size();
    char *ans = new char[_size];
    strcpy(ans, ret.c_str());
    int tmp;//记录每次写入的长度
    const char* p = ans;
    while(1) {
        tmp = write(socket_fd, p, _size);
        if(tmp < 0) {
            if(errno != EAGAIN) {
                printf("write failed!\n");
                exit(1);
            }
            sleep(10);//过一段时间尝试再次写入
            continue;
        }
        if (tmp == _size) {//已经读完
            break;
        }
        //没读完
        _size = _size - tmp;
        p = p + tmp;//移动指针，指向下一个该写入的数据
        

    }
    // if(write(socket_fd, ans, _size) < 0) {
    //     printf("write failed!\n");
    //     exit(1);
    // }
    printf("process data over!\n");
    delete ans;

}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        printf("format error!\n");
		printf("usage: server <address> <port>\n");
		exit(1);
    }
    //1.首先需要建立一个eventpool，当然，需要用文件描述符标识
    int eventpool_fd = epoll_create(MAX_EVENT+10); //这个参数就是出初始化红黑树节点的个数，必须大于max_event
    if(eventpool_fd < 0) {
        perror("epoll create error:");
        exit(1);
    }
    FileRAII _event_fd(eventpool_fd);//RTII
    int event_fd = _event_fd.get();
    //2.接下来进行socket的创建和绑定及监听工作
    //int server_fd = init_server(argv);
    struct sockaddr_in server_addr;//地址
    server_addr.sin_family = AF_INET;//ipv4协议
    server_addr.sin_port = htons(atoi(argv[2]));//绑定端口
    if(!inet_aton(argv[1], (struct in_addr*)&server_addr.sin_addr.s_addr)) {//将点分十进制ip地址转换为对应形式
        perror("invalid ip addr:");
		exit(1); 
    }
    //1.创建套接字
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        perror("socket create fail:");
        exit(1);
    }
    FileRAII _ser_fd(server_fd);//！！！利用RAII
    int ser_fd = _ser_fd.get();
    //2.端口重用，如果不这样设置的话，建立TCP连接后，将无法再监听其他的客户端
	int reuse = 0x01;
	if(setsockopt(ser_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(int)) < 0) {
		perror("setsockopt error");
		// close(server_fd);
		exit(1); 
	}
    //3.绑定套接字(ip,端口)
    if(bind(ser_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error:");
        // close(server_fd);
        exit(1);
    }
    //4.开始监听
    if(listen(ser_fd, 127) < 0) {
        perror("listen error");
        //close(server_fd);
        exit(1);
    }    

    struct epoll_event event;//定义插入时的epoll事件
    struct epoll_event wait_event[MAX_EVENT];//定义最终返回的触发的事件描述符及其他信息
    int flag;//阻塞非阻塞标志
    event.data.fd = ser_fd;
    event.events = EPOLLIN | EPOLLET;//建立的连接请求对应的server_fd只能读，不能修改，必须为边沿触发，为了一次性处理全部连接请求，以及防止客户端建立连接过程中发送RST
    flag = fcntl(ser_fd, F_GETFL);//返回阻塞类型
    flag |= O_NONBLOCK;
    fcntl(ser_fd, F_SETFL, flag);
    //3.将server_fd放入eventpool的就绪队列，等待请求连接
    if(epoll_ctl(event_fd, EPOLL_CTL_ADD, ser_fd, &event) < 0) {
        perror("epoll_ctl error:");
        //close(event_fd);

        exit(1);
    }
    //客户端的ip地址和端口
    struct sockaddr_in client;
    int size_sock = sizeof(struct sockaddr_in);//地址长度
    char buf[1024];//保存接受数据和发送数据，使用前记得清空
    
    FileRAII _sock_fd[MAX_EVENT+10];
    int num = 0;//文件数目
    while(1) {
        //4.等待事件触发。这里有一个问题，虽然连接socket最好使用水平触发，但他没法和其他socket区分开，因此只能都使用边沿触发 XXX理解错了！！！
        int fd_size = epoll_wait(event_fd, wait_event, MAX_EVENT, -1);
        if(fd_size < 0) {
            perror("perror wait error:");
            //close(server_fd);
            //close(event_fd);
            exit(1);
        }
        for(int i = 0; i < fd_size; i++) {
            if(wait_event[i].data.fd == ser_fd) {//说明是建立连接的请求  
                //得到发送文件对应的描述符和其中的ip和端口
                int socket_fd;
                while((socket_fd = accept(ser_fd, (struct sockaddr*)&client, (socklen_t *)&size_sock)) > 0) {//循环处理所有连接
                    _sock_fd[num].change(socket_fd);
                    int sock_fd = _sock_fd[num].get();
                    num++;
                    printf("connected with ip: %s and port: %d\n", inet_ntop(AF_INET, &client.sin_addr, buf, 1024), ntohs(client.sin_port));
                    //设置socket为非阻塞机制
                    flag = fcntl(sock_fd, F_GETFL);//返回阻塞类型
                    flag |= O_NONBLOCK;
                    fcntl(sock_fd, F_SETFL, flag);
                    event.events = EPOLLIN | EPOLLET;//此时必须设置为边沿触发，提高效率
                    event.data.fd = sock_fd;
                    if(epoll_ctl(event_fd, EPOLL_CTL_ADD, sock_fd, &event) < 0) {
                        perror("epoll_ctl error:");
                        break;
                    }
                }
                if(socket_fd < 0) {//判断出错原因是否为连接全部建完
                    if(errno != EAGAIN) {//不是读完，则出错
                        perror("accept error:");
                    }
                    // continue;
                }
                continue;
            }
            else {
                //如果不是连接请求，那就是socket数据包的读写请求
                process_data(wait_event[i].data.fd);
                //sleep(5);
            }
        }
    }
    return 0;

}
