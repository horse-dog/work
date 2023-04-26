#ifndef CONN_H
#define CONN_H

#include <arpa/inet.h>

#include "fdwrapper.h"

// 用于管理连接的类
class conn {
 public:
  conn();
  ~conn();
  // 初始化客户端
  void init_clt(int sockfd, const sockaddr_in& client_addr);
  // 初始化服务端
  void init_srv(int sockfd, const sockaddr_in& server_addr);
  void reset();
  RET_CODE read_clt();
  RET_CODE write_clt();
  RET_CODE read_srv();
  RET_CODE write_srv();

 public:
  // 缓冲区大小
  static const int BUF_SIZE = 2048;

  char* m_clt_buf;
  int m_clt_read_idx;
  int m_clt_write_idx;
  sockaddr_in m_clt_address;
  int m_cltfd;

  char* m_srv_buf;
  int m_srv_read_idx;
  int m_srv_write_idx;
  sockaddr_in m_srv_address;
  int m_srvfd;

  bool m_srv_closed;
};

#endif
