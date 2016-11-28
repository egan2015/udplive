#ifndef __socket_layer_h__
#define __socket_layer_h__
/**
 * 对socket进行封装，要实现终端多网卡绑定，实现轮询式负载均衡
 * 对上层提供数据发送和接收服务，接收数据通过回调方式通知上层应用
 */

#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include "list.h"

#ifndef SO_BINDTODEVICE
#define SO_BINDTODEVICE 25
#endif

typedef enum
{
  VLSTP_SOCKET_CLOSE = 0,
  VLSTP_SOCKET_IDEL,
  VLSTP_SOCKET_BUSY
} socket_state_t;

struct socket
{
  /*存储socket的链表*/
  struct list_head list;
  /*socket handle*/
  int sockfd;
  /*socket 绑定的地址*/
  struct sockaddr bind_addr;
  /*绑定设备名*/
  char ifname[255];
  /*socket state*/
  socket_state_t state;
};

typedef struct fdset_s fdset_t;
struct fdset_s {
  fd_set fdread;
  fd_set fdwrite;
  fd_set fdexcept;
  int size;
  int numready;
};

void fd_create(fdset_t *fdSet);
int  fd_select(fdset_t *fdSet, unsigned long ms);
int  fd_ready_read(int fd, fdset_t* fdSet);
int  fd_ready_write(int fd, fdset_t* fdSet );
void fd_set_read(int fd, fdset_t* fdSet);
void fd_set_write(int fd, fdset_t* fdSet);
int  fd_set_noblock(int fd, int noblock );
int  fd_bind_local ( int fd, char *addr , int port );
int  fd_bind_device ( int fd , char *devname );
int  fd_create_socket( int proto, char *devname , char * local_addr, int port );
int  fd_write( int fd, unsigned char * data, int len );
int  fd_read ( int fd, unsigned char * buf,  int len );


#endif