#ifndef CONN_H
#define CONN_H

#include <arpa/inet.h>

#include "fdwrapper.h"

// 用于管理与远程服务器的连接的类
class conn {
 public:
  // 构造函数
  conn();

  // 析构函数
  ~conn();

  // 初始化客户端数据：描述符和地址
  void init_clt(int sockfd, const sockaddr_in& client_addr);
  
  // 初始化远程服务端数据：描述符和地址
  void init_srv(int sockfd, const sockaddr_in& server_addr);
  
  // 重置一些数据
  void reset();
  
  // 从m_clt_fd的内核接收缓冲区读取数据到客户端buffer，会更新客户端buffer读指针
  RET_CODE read_clt();
  
  // 将服务端buffer的数据写入到m_clt_fd的内核发送缓冲区，会更新服务端buffer写指针
  RET_CODE write_clt();
  
  // 从m_srv_fd的内核接收缓冲区读取数据到服务端buffer，会更新服务端buffer读指针
  RET_CODE read_srv();
  
  // 将客户端buffer的数据写入到m_srv_fd的内核发送缓冲区，会更新客户端buffer写指针
  RET_CODE write_srv();

 public:
  // 缓冲区大小
  static const int BUF_SIZE = 2048;

  char* m_clt_buf;    /*> 客户端buffer */
  int m_clt_read_idx; /*> 客户端buffer读指针 */
  int m_clt_write_idx;/*> 客户端buffer写指针 */
  sockaddr_in m_clt_address; /*> 客户端地址 */
  int m_cltfd;  /*> 客户端描述符 */

  char* m_srv_buf;    /*> 服务端buffer */
  int m_srv_read_idx; /*> 服务端buffer读指针 */
  int m_srv_write_idx;/*> 服务端buffer写指针 */
  sockaddr_in m_srv_address;/*> 远程服务器的地址 */
  int m_srvfd;  /*> 远程服务器描述符 */

  bool m_srv_closed;  /*> 远程服务器是否关闭 */
};

#endif
