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
#include <unistd.h>

static int quit = 0;

// void setNonBlock(int fd) {
//   auto oldOpt = fcntl(fd, F_GETFL);
//   auto newOpt = oldOpt | O_NONBLOCK;
//   fcntl(fd, F_SETFL, newOpt);
// }

// void add4Read(int epfd, int fd, int os = 0) {
//   epoll_event ev;
//   memset(&ev, '\0', sizeof(ev));
//   ev.data.fd = fd;
//   ev.events = EPOLLIN | EPOLLET;
//   if (os != 0) ev.events |= EPOLLONESHOT;
//   setNonBlock(fd);
//   epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
// }

// int main() {
//   const char* ipAddr = "127.0.0.1";
//   int port = 12345;
//   int ret;

//   int epfd = epoll_create(5);
//   assert(epfd != -1);

//   add4Read(epfd, STDIN_FILENO);

//   int listenfd = socket(AF_INET, SOCK_STREAM, 0);
//   assert(listenfd != -1);

//   sockaddr_in listenAddr;
//   memset(&listenAddr, '\0', sizeof(listenAddr));
//   listenAddr.sin_family = AF_INET;
//   listenAddr.sin_port = htons(port);
//   listenAddr.sin_addr.s_addr = inet_addr(ipAddr);

//   ret = bind(listenfd, (sockaddr*)(&listenAddr), sizeof(listenAddr));
//   assert(ret != -1);
//   add4Read(epfd, listenfd);

//   ret = listen(listenfd, 3);
//   assert(ret != -1);

//   epoll_event events[1024];

//   while (!quit) {
//     int eventCnt = epoll_wait(epfd, events, 1024, -1);

//     if (eventCnt == -1 && errno == EINTR) continue;

//     if (eventCnt < 0) {
//       perror("epoll_wait error");
//       exit(1);
//     }

//     for (int i = 0; i < eventCnt; i++) {
//       int sockfd = events[i].data.fd;
//       if (sockfd == STDIN_FILENO) {
//         char cmd[1024];
//         memset(cmd, '\0', sizeof(cmd));
//         int x = read(STDIN_FILENO, cmd, sizeof(cmd));
//         if (strcmp(cmd, "q\r\n") == 0 || strcmp(cmd, "q\n") == 0) {
//           quit = true;
//           break;
//         } else {
//           continue;
//         }
//       }
//       if (sockfd == listenfd) {
//         sockaddr_in cliAddr;
//         socklen_t cliAddrLen = sizeof(cliAddr);
//         int clifd = accept(sockfd, (sockaddr*)(&cliAddr), &cliAddrLen);
//         if (clifd == -1 && errno == EAGAIN) {
//           i--;
//           continue;
//         }
//         if (clifd < 0) {
//           perror("accpet error");
//           exit(0);
//         }
//         add4Read(epfd, clifd);
//       } else {
//         char buffer[5];
//         int numOfRecvBytes;
//         while (true) {
//           memset(buffer, '\0', sizeof(buffer));
//           numOfRecvBytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
//           if (numOfRecvBytes == -1 && errno == EAGAIN) break;
//           if (numOfRecvBytes < 0) {
//             perror("recv error");
//             exit(0);
//           }
//           if (numOfRecvBytes == 0) {
//             epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
//             close(sockfd);
//             break;
//           }

//           int numOfSendBytes = send(sockfd, buffer, numOfRecvBytes, 0);
//           if (numOfRecvBytes != numOfSendBytes) {
//             perror("send error");
//             exit(0);
//           }
//         }
//       }
//     }
//   }

//   close(listenfd);
//   close(epfd);
//   printf("echo server exit with 0, bye~\n");
//   return 0;
// }

class Base {
public:
  virtual void __inner() const {
    printf("Base::__inner\n");
  }
  virtual void invoke() const {
    printf("Base::invoke\n");
    __inner();
  }
};

class Derived : public Base {
public:
  void __inner() const override {
    printf("Derived::__inner\n");
  }
};

int main(int argc, const char* argv[]) {
  Base* p = new Derived();
  p->invoke();
  return 0;
}