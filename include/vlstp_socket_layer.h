#ifndef __vlstp_socket_layer_h__
#define __vlstp_socket_layer_h__
/**
 * 对socket进行封装，要实现终端多网卡绑定，实现轮询式负载均衡
 * 对上层提供数据发送和接收服务，接收数据通过回调方式通知上层应用
 */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include "list.h"

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


#endif