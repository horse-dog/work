#ifndef SRVMGR_H
#define SRVMGR_H

#include <arpa/inet.h>

#include <map>

#include "conn.h"
#include "fdwrapper.h"

using std::map;

// 主机类
class host {
 public:
  char m_hostname[1024];/*> 主机ip */
  int m_port;           /*> 主机端口 */
  int m_conncnt;        /*> 最多支持的连接数量 */
};

// 管理者类，用于管理多个与远程服务器的连接
class mgr {
 public:
  /**
   * @brief 构造函数
   * 
   * @param epollfd epfd
   * @param srv 管理的主机
   */
  mgr(int epollfd, const host& srv);
  
  // 析构函数
  ~mgr();

  // 连接到远端address，并返回连接的套接字描述符
  int conn2srv(const sockaddr_in& address);
  
  // 获取一个与远程服务器的连接
  conn* pick_conn(int sockfd);
  
  // 释放一个与远程服务器的连接
  void free_conn(conn* connection);

  // 获取正在使用的连接的数量
  int get_used_conn_cnt();

  // 回收再利用m_freed中管理的与远程服务器的连接
  void recycle_conns();
  
  // 处理文件描述符fd上发生的事件，type=READ代表可读事件，type=WRITE代表可写事件
  RET_CODE process(int fd, OP_TYPE type);

 private:
  static int m_epollfd;     /*> epfd */
  map<int, conn*> m_conns;  /*> 初始具有的连接，字典，键为这个连接的客户端描述符，值为指向这个连接的指针 */
  map<int, conn*> m_used;   /*> 正在使用的连接，字典，键为这个连接的客户端描述符或服务端描述符，值为指向这个连接的指针 */
  map<int, conn*> m_freed;  /*> 空闲的连接，字典 */
  host m_logic_srv;         /*> 远程服务器主机信息 */
};

#endif
