#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

int timeout_connect(const char* ip, int port, int time) {
  int ret = 0;
  struct sockaddr_in address;
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  inet_pton(AF_INET, ip, &address.sin_addr);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd >= 0);
  
  struct timeval timeout;
  timeout.tv_sec = time;
  timeout.tv_usec = 0;
  
  ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
  assert(ret != -1);

  ret = connect(sockfd, (struct sockaddr*)(&address), sizeof(address));
  if (ret == -1) {
    if (errno == EINPROGRESS) {
      printf("connecting timeout, process timeout logic \n");
      return -1;
    }
    printf("error occur when connecting to server\n");
    return -1;
  }
  return sockfd;
}

int main(int argc, char* argv[]) {

  if (argc != 3) {
    printf("usage: %s ip_address port_number\n", basename(argv[0]));
    return 1;
  }

  const char* ip = argv[1];
  int port = atoi(argv[2]);
  
  int sockfd = timeout_connect(ip, port, 10);
  if (sockfd < 0)
    return 1;
  return 0;

}