#include "conn.h"

#include <errno.h>
#include <string.h>

#include <exception>

#include "fdwrapper.h"
#include "log.h"

conn::conn() {
  m_srvfd = -1;
  m_clt_buf = new char[BUF_SIZE];
  if (!m_clt_buf) {
    throw std::exception();
  }
  m_srv_buf = new char[BUF_SIZE];
  if (!m_srv_buf) {
    throw std::exception();
  }
  reset();
}

conn::~conn() {
  delete[] m_clt_buf;
  delete[] m_srv_buf;
}

void conn::init_clt(int sockfd, const sockaddr_in& client_addr) {
  m_cltfd = sockfd;
  m_clt_address = client_addr;
}

void conn::init_srv(int sockfd, const sockaddr_in& server_addr) {
  m_srvfd = sockfd;
  m_srv_address = server_addr;
}

void conn::reset() {
  m_clt_read_idx = 0;
  m_clt_write_idx = 0;
  m_srv_read_idx = 0;
  m_srv_write_idx = 0;
  m_srv_closed = false;
  m_cltfd = -1;
  memset(m_clt_buf, '\0', BUF_SIZE);
  memset(m_srv_buf, '\0', BUF_SIZE);
}

RET_CODE conn::read_clt() {
  int bytes_read = 0;
  while (true) {
    // 缓冲区读满了，则返回 BUFFER_FULL
    if (m_clt_read_idx >= BUF_SIZE) {
      log(LOG_ERR, __FILE__, __LINE__, "%s",
          "the client read buffer is full, let server write");
      return BUFFER_FULL;
    }

    // 接收数据到m_clt_buf
    bytes_read =
        recv(m_cltfd, m_clt_buf + m_clt_read_idx, BUF_SIZE - m_clt_read_idx, 0);
    
    if (bytes_read == -1) {
      // 这代表内核缓冲区被读空了
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      // 否则代表发生了IO错误
      return IOERR;
    } else if (bytes_read == 0) { // 这代表客户端正常关闭了连接
      return CLOSED;
    }

    // 更新读指针
    m_clt_read_idx += bytes_read;
  }
  return ((m_clt_read_idx - m_clt_write_idx) > 0) ? OK : NOTHING;
}

RET_CODE conn::read_srv() {
  int bytes_read = 0;
  while (true) {
    // 缓冲区读满了，则返回 BUFFER_FULL
    if (m_srv_read_idx >= BUF_SIZE) {
      log(LOG_ERR, __FILE__, __LINE__, "%s",
          "the server read buffer is full, let client write");
      return BUFFER_FULL;
    }

    // 接收数据到m_srv_buf
    bytes_read =
        recv(m_srvfd, m_srv_buf + m_srv_read_idx, BUF_SIZE - m_srv_read_idx, 0);
    if (bytes_read == -1) {
      // 这代表内核缓冲区被读空了
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      // 否则代表发生了IO错误
      return IOERR;
    } else if (bytes_read == 0) { // 这代表服务端正常关闭了连接
      log(LOG_ERR, __FILE__, __LINE__, "%s",
          "the server should not close the persist connection");
      return CLOSED;
    }

    // 更新读指针
    m_srv_read_idx += bytes_read;
  }
  return ((m_srv_read_idx - m_srv_write_idx) > 0) ? OK : NOTHING;
}

RET_CODE conn::write_srv() {
  int bytes_write = 0;
  while (true) {
    // 缓冲区数据被读空了，重置读写指针为0，返回 BUFFER_EMPTY
    if (m_clt_read_idx <= m_clt_write_idx) {
      m_clt_read_idx = 0;
      m_clt_write_idx = 0;
      return BUFFER_EMPTY;
    }

    // 发送用户缓冲区的数据给远程服务器
    bytes_write = send(m_srvfd, m_clt_buf + m_clt_write_idx,
                       m_clt_read_idx - m_clt_write_idx, 0);
    if (bytes_write == -1) {
      // 这代表内核发送缓冲区满了，过一会儿再重新发送，返回TRY_AGAIN
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return TRY_AGAIN;
      }
      // 否则代表发生了IO错误
      log(LOG_ERR, __FILE__, __LINE__, "write server socket failed, %s",
          strerror(errno));
      return IOERR;
    } else if (bytes_write == 0) {  // 这代表服务端正常关闭了连接
      return CLOSED;
    }

    // 更新写指针
    m_clt_write_idx += bytes_write;
  }
}

RET_CODE conn::write_clt() {
  int bytes_write = 0;
  while (true) {
    // 缓冲区数据被读空了，重置读写指针为0，返回 BUFFER_EMPTY
    if (m_srv_read_idx <= m_srv_write_idx) {
      m_srv_read_idx = 0;
      m_srv_write_idx = 0;
      return BUFFER_EMPTY;
    }

    // 发送服务端缓冲区的数据给用户
    bytes_write = send(m_cltfd, m_srv_buf + m_srv_write_idx,
                       m_srv_read_idx - m_srv_write_idx, 0);
    if (bytes_write == -1) {
      // 这代表内核发送缓冲区满了，过一会儿再重新发送，返回TRY_AGAIN
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return TRY_AGAIN;
      }
      // 否则代表发生了IO错误
      log(LOG_ERR, __FILE__, __LINE__, "write client socket failed, %s",
          strerror(errno));
      return IOERR;
    } else if (bytes_write == 0) {  // 这代表客户端正常关闭了连接
      return CLOSED;
    }

    // 更新写指针
    m_srv_write_idx += bytes_write;
  }
}
