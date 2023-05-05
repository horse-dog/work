#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <vector>

#include "conn.h"
#include "log.h"
#include "mgr.h"
#include "processpool.h"

using std::vector;

// 程序版本
static const char* version = "1.0";

// 打印程序使用方式
static void usage(const char* prog) {
  log(LOG_INFO, __FILE__, __LINE__, "usage: %s [-h] [-v] [-f config_file]",
      prog);
}

// 程序入口
int main(int argc, char* argv[]) {

  char cfg_file[1024];
  memset(cfg_file, '\0', 100);
  int option;

  // argc = 4;
  // const char* xx[] = {"-x", "-f", "./config.xml"};
  // argv = (char**)xx;

  // getopt函数用于解析参数，其中f:表示选项f后面必须带有参数，x、v、h选项是普通选项，不需要附加参数
  while ((option = getopt(argc, argv, "f:xvh")) != -1) {
    switch (option) {
      // 如果当前解析的选项是 -x，设置日志等级为debug
      case 'x': {
        set_loglevel(LOG_DEBUG);
        break;
      }
      // 如果当前解析的选项是 -v，在日志中输出程序版本信息
      case 'v': {
        log(LOG_INFO, __FILE__, __LINE__, "%s %s", basename(argv[0]), version);
        return 0;
      }
      // 如果当前解析的选项是 -h，查看命令帮助
      case 'h': {
        usage(basename(argv[0]));
        return 0;
      }
      // 如果当前解析的选项是 -f，-f之后的参数会被读取到optarg这个外部变量中
      case 'f': {
        // 将-f之后的参数拷贝到cfg_file，这个参数其实就是配置文件的路径
        memcpy(cfg_file, optarg, strlen(optarg));
        break;
      }
      // 如果当前解析的选项是未知选项，输出错误日志
      case '?': {
        log(LOG_ERR, __FILE__, __LINE__, "un-recognized option %c", option);
        usage(basename(argv[0]));
        return 1;
      }
    }
  }

  // 确保配置文件名字符串不为空
  if (cfg_file[0] == '\0') {
    log(LOG_ERR, __FILE__, __LINE__, "%s", "please specifiy the config file");
    return 1;
  }

  // 只读方式打开配置文件
  int cfg_fd = open(cfg_file, O_RDONLY);
  if (!cfg_fd) {
    log(LOG_ERR, __FILE__, __LINE__, "read config file met error: %s",
        strerror(errno));
    return 1;
  }

  // 读取配置文件的元信息
  struct stat ret_stat;
  if (fstat(cfg_fd, &ret_stat) < 0) {
    log(LOG_ERR, __FILE__, __LINE__, "read config file met error: %s",
        strerror(errno));
    return 1;
  }

  // 将配置文件中的内容读取到用户缓冲区buf中
  char* buf = new char[ret_stat.st_size + 1];
  memset(buf, '\0', ret_stat.st_size + 1);
  ssize_t read_sz = read(cfg_fd, buf, ret_stat.st_size);
  if (read_sz < 0) {
    log(LOG_ERR, __FILE__, __LINE__, "read config file met error: %s",
        strerror(errno));
    return 1;
  }

  vector<host> balance_srv;
  vector<host> logical_srv;
  host tmp_host;
  memset(tmp_host.m_hostname, '\0', 1024);
  char* tmp_hostname;
  char* tmp_port;
  char* tmp_conncnt;
  bool opentag = false;
  char* tmp = buf;  // tmp初始化指向配置文件的内容的开始位置
  char* tmp2 = NULL;
  char* tmp3 = NULL;
  char* tmp4 = NULL;
  
  // strpbrk(str1,str2)依次检验字符串str1中的字符，当被检验字符在字符串str2中也包含时，则停止检验，并返回该字符位置。
  // 因此tmp2 = strpbrk(tmp, "\n") 即寻找tmp中第一个换行的位置
  // 这个while循环的本质就是逐行解析配置文件，会把每一行末尾的\n替换为\0。strtok函数也能达到类似效果
  while (tmp2 = strpbrk(tmp, "\n")) {
    *tmp2++ = '\0';
    // 当前行tmp中包含有"<logical_host>"
    if (strstr(tmp, "<logical_host>")) {
      if (opentag) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      opentag = true;
    } else if (strstr(tmp, "</logical_host>")) {
      if (!opentag) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      // 向logical_srv中添加一个host
      logical_srv.push_back(tmp_host);
      memset(tmp_host.m_hostname, '\0', 1024);
      opentag = false;
    } else if (tmp3 = strstr(tmp, "<name>")) {
      tmp_hostname = tmp3 + 6;
      tmp4 = strstr(tmp_hostname, "</name>");
      if (!tmp4) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      *tmp4 = '\0';
      // 读取主机ip地址
      memcpy(tmp_host.m_hostname, tmp_hostname, strlen(tmp_hostname));
    } else if (tmp3 = strstr(tmp, "<port>")) {
      tmp_port = tmp3 + 6;
      tmp4 = strstr(tmp_port, "</port>");
      if (!tmp4) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      *tmp4 = '\0';
      // 读取主机端口号
      tmp_host.m_port = atoi(tmp_port);
    } else if (tmp3 = strstr(tmp, "<conns>")) {
      tmp_conncnt = tmp3 + 7;
      tmp4 = strstr(tmp_conncnt, "</conns>");
      if (!tmp4) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      *tmp4 = '\0';
      // 读取主机最大支持的连接数量
      tmp_host.m_conncnt = atoi(tmp_conncnt);
    } else if (tmp3 = strstr(tmp, "Listen")) {  // 获取负载均衡地址
      tmp_hostname = tmp3 + 6;
      tmp4 = strstr(tmp_hostname, ":");
      if (!tmp4) {
        log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
        return 1;
      }
      *tmp4++ = '\0';
      tmp_host.m_port = atoi(tmp4);
      memcpy(tmp_host.m_hostname, tmp3, strlen(tmp3));
      balance_srv.push_back(tmp_host);
      memset(tmp_host.m_hostname, '\0', 1024);
    }
    tmp = tmp2;
  }

  // 负载均衡地址和逻辑地址都不能是0个
  if (balance_srv.size() == 0 || logical_srv.size() == 0) {
    log(LOG_ERR, __FILE__, __LINE__, "%s", "parse config file failed");
    return 1;
  }

  // 获取负载均衡IP地址
  const char* ip = balance_srv[0].m_hostname;
  // 获取负载均衡主机端口号
  int port = balance_srv[0].m_port;

  // 创建监听套接字
  int listenfd = socket(PF_INET, SOCK_STREAM, 0);
  assert(listenfd >= 0);

  // 设置监听套接字的ip地址和端口
  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  inet_pton(AF_INET, ip, &address.sin_addr);
  address.sin_port = htons(port);

  // 为监听套接字绑定指定的地址
  ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address));
  assert(ret != -1);

  // 开始监听
  ret = listen(listenfd, 5);
  assert(ret != -1);

  // 有多少个逻辑地址，就开启多少个子进程
  processpool<conn, host, mgr>* pool =
      processpool<conn, host, mgr>::create(listenfd, logical_srv.size());
  if (pool) {
    // 子进程开始运行
    pool->run(logical_srv);
    // 在这里delete（感觉不是很好）
    delete pool;
  }

  // 关闭监听套接字
  close(listenfd);
  // 程序正常退出
  return 0;
}
